#include "Renderer.h"
#include "../Shader/ShaderStorage.h"
#include "../Texture/TextureStorage.h"
#include "../Geometry/QuadGeometry.h"

#include <glad/glad.h>
#include <algorithm>

namespace Engine {
	CameraComponent* Renderer::s_ActiveCamera = nullptr;
	std::vector<std::unique_ptr<IRenderBatch>> Renderer::s_Batches;

	std::unordered_map<Renderer::MeshBatchKey, MeshBatch*, Renderer::MeshBatchKeyHasher> Renderer::s_MeshBatchLookup;
	std::unordered_map<Renderer::ParticleBatchKey, ParticleBatch*, Renderer::ParticleBatchKeyHasher> Renderer::s_ParticleBatchLookup;
	std::unordered_map<Renderer::UIBatchKey, UIBatch*, Renderer::UIBatchKeyHasher> Renderer::s_UIBatchLookup;

	int Renderer::s_ViewportWidth = 800;
	int Renderer::s_ViewportHeight = 600;


	void Renderer::Init() {
		glEnable(GL_DEPTH_TEST);

		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		try {
			ShaderStorage::GetInstance().Load("BasicShader", "assets/shaders/BasicVertexShader.glsl", "assets/shaders/BasicFragmentShader.glsl");
			ShaderStorage::GetInstance().Load("ParticleShader", "assets/shaders/ParticleVertexShader.glsl", "assets/shaders/ParticleFragmentShader.glsl");
			ShaderStorage::GetInstance().Load("UIShader", "assets/shaders/UIVertexShader.glsl", "assets/shaders/UIFragmentShader.glsl");
		}
		catch (const std::runtime_error& e) {
			std::cerr << "Error loading default shaders in Renderer::Init: " << e.what() << std::endl;
		}

		s_Batches.clear();
		s_MeshBatchLookup.clear();
		s_ParticleBatchLookup.clear();
		s_UIBatchLookup.clear();
		s_ActiveCamera = nullptr;
	}

	void Renderer::Shutdown() {
		s_Batches.clear();
		s_MeshBatchLookup.clear();
		s_ParticleBatchLookup.clear();
		s_UIBatchLookup.clear();

		ShaderStorage::GetInstance().Shutdown();
		TextureStorage::GetInstance().Shutdown();
		QuadGeometry::Shutdown();
	}

	void Renderer::SetViewportSize(int width, int height) {
		s_ViewportWidth = width;
		s_ViewportHeight = height;

		glViewport(0, 0, s_ViewportWidth, s_ViewportHeight);
	}

	void Renderer::BeginScene(CameraComponent& camera) {
		s_ActiveCamera = &camera;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& batch : s_Batches) {
			batch->Begin();
		}
	}

	void Renderer::EndScene() {
		std::stable_sort(s_Batches.begin(), s_Batches.end(), [](const std::unique_ptr<IRenderBatch>& a, const std::unique_ptr<IRenderBatch>& b) {
			if (!a->HasContent()) return false;
			if (!b->HasContent()) return true;
			if (a->GetQueue() != b->GetQueue())
				return static_cast<int>(a->GetQueue()) < static_cast<int>(b->GetQueue());
			return a->GetSortOrder() < b->GetSortOrder();
		});

		glm::mat4 view = s_ActiveCamera->GetViewMatrix();
		glm::mat4 projection = s_ActiveCamera->GetProjectionMatrix();

		glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

		glm::mat4 uiView = glm::mat4(1.0f);
		glm::mat4 uiProjection = glm::ortho(
			0.0f, static_cast<float>(s_ViewportWidth),
			static_cast<float>(s_ViewportHeight), 0.0f,
			-1.0f, 1.0f);

		RenderQueue current = RenderQueue::Opaque;
		ApplyRenderState(current);

		for (auto& batch : s_Batches) {
			if (!batch->HasContent())
				continue;

			RenderQueue queue = batch->GetQueue();
			if (queue != current) {
				ApplyRenderState(queue);
				current = queue;
			}

			batch->End();
			switch (queue) {
			case RenderQueue::Skybox:
				batch->Flush(skyboxView, projection);
				break;
			case RenderQueue::UI:
				batch->Flush(uiView, uiProjection);
				break;
			default:
				batch->Flush(view, projection);
				break;
			}
		}

		ApplyRenderState(RenderQueue::Opaque); 
		s_ActiveCamera = nullptr;
	}

	template<typename BatchT, typename KeyT, typename HasherT, typename FactoryFn>
	BatchT* Renderer::GetOrCreateBatch(std::unordered_map<KeyT, BatchT*, HasherT>& lookup, const KeyT& key, FactoryFn&& factory) {
		auto it = lookup.find(key);
		BatchT* batch = (it != lookup.end()) ? it->second : nullptr;

		if (!batch || batch->IsFull()) {
			std::unique_ptr<BatchT> owned = factory();
			batch = owned.get();
			batch->Begin();
			s_Batches.push_back(std::move(owned));
			lookup[key] = batch;
		}

		return batch;
	}

	void Renderer::Submit(const Mesh& mesh, TransformComponent& transform, MaterialComponent& material) {
		auto* shader = material.shader.get();
		MeshBatchKey key{ shader, &mesh, material.texture.get(), material.queue};

		MeshBatch* batch = GetOrCreateBatch(s_MeshBatchLookup, key, [shader]() {
			return std::make_unique<MeshBatch>(shader);
		});

		batch->Submit(mesh, transform, material);
	}

	void Renderer::SubmitParticle(const Particle& particle, ShaderProgram* shader, const Texture* texture, RenderQueue queue) {
		if (!shader)
			return;

		ParticleBatchKey key{ shader, texture, queue };

		ParticleBatch* batch = GetOrCreateBatch(s_ParticleBatchLookup, key, [shader, texture, queue]() {
			return std::make_unique<ParticleBatch>(shader, texture, queue);
		});

		batch->Submit(particle);
	}

	void Renderer::SubmitUI(const UIBatch::InstanceData& instance, ShaderProgram* shader, const Texture* texture, int zOrder) {
		if (!shader)
			return;

		UIBatchKey key{ shader, texture, zOrder };

		UIBatch* batch = GetOrCreateBatch(s_UIBatchLookup, key, [shader, texture, zOrder]() {
			return std::make_unique<UIBatch>(shader, texture, zOrder);
		});

		batch->Submit(instance);
	}

	void Renderer::ApplyRenderState(RenderQueue queue) {
		switch (queue) {
		case RenderQueue::Opaque: 
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			break;
		case RenderQueue::Transparent:
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			break;
		case RenderQueue::Particles:
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case RenderQueue::Skybox:
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
			break;
		case RenderQueue::UI:
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
	}
}
