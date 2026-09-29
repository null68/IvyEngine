#pragma once

#include "IRenderBatch.h"
#include "MeshBatch.h"
#include "ParticleBatch.h"
#include "UIBatch.h"
#include "../Mesh/Mesh.h"

#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/MaterialComponent.h"
#include "../../Particles/Particle.h"

namespace Engine {
	class Renderer {
	public:
		static void Init();
		static void Shutdown();
		
		static void SetViewportSize(int width, int height);

		static int GetViewportWidth() { return s_ViewportWidth; };
		static int GetViewportHeight() { return s_ViewportHeight; };

		static void BeginScene(CameraComponent& camera);
		static void EndScene();

		static void Submit(const Mesh& mesh, TransformComponent& transform, MaterialComponent& material);
		static void SubmitParticle(const Particle& particle, ShaderProgram* shader, const Texture* texture, RenderQueue queue = RenderQueue::Particles);
		static void SubmitUI(const UIBatch::InstanceData& instance, ShaderProgram* shader, const Texture* texture, int zOrder);
	private:
		struct MeshBatchKey {
			ShaderProgram* shader;
			const Mesh* mesh;
			const Texture* texture;
			RenderQueue queue;

			bool operator==(const MeshBatchKey& other) const {
				return shader == other.shader && mesh == other.mesh && texture == other.texture && queue == other.queue;
			}
		};
		struct MeshBatchKeyHasher {
			size_t operator()(const MeshBatchKey& key) const {
				size_t h1 = std::hash<void*>()(static_cast<void*>(key.shader));
				size_t h2 = std::hash<const void*>()(static_cast<const void*>(key.mesh));
				size_t h3 = std::hash<const void*>()(static_cast<const void*>(key.texture));
				return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (static_cast<size_t>(key.queue) << 3);
			}
		};

		struct ParticleBatchKey {
			ShaderProgram* shader;
			const Texture* texture;
			RenderQueue queue;

			bool operator==(const ParticleBatchKey& other) const {
				return shader == other.shader && texture == other.texture && queue == other.queue;
			}
		};
		struct ParticleBatchKeyHasher {
			size_t operator()(const ParticleBatchKey& key) const {
				size_t h1 = std::hash<void*>()(static_cast<void*>(key.shader));
				size_t h2 = std::hash<const void*>()(static_cast<const void*>(key.texture));
				size_t h3 = std::hash<int>()(static_cast<int>(key.queue));
				return h1 ^ (h2 << 1) ^ (h3 << 2);
			}
		};

		struct UIBatchKey {
			ShaderProgram* shader;
			const Texture* texture;
			int zOrder;

			bool operator==(const UIBatchKey& other) const {
				return shader == other.shader && texture == other.texture && zOrder == other.zOrder;
			}
		};
		struct UIBatchKeyHasher {
			size_t operator()(const UIBatchKey& key) const {
				size_t h1 = std::hash<void*>()(static_cast<void*>(key.shader));
				size_t h2 = std::hash<const void*>()(static_cast<const void*>(key.texture));
				size_t h3 = std::hash<int>()(key.zOrder);
				return h1 ^ (h2 << 1) ^ (h3 << 2);
			}
		};

		template<typename BatchT, typename KeyT, typename HasherT, typename FactoryFn>
		static BatchT* GetOrCreateBatch(std::unordered_map<KeyT, BatchT*, HasherT>& lookup, const KeyT& key, FactoryFn&& factory);

		static void ApplyRenderState(RenderQueue queue);

		static CameraComponent* s_ActiveCamera;
		static std::vector<std::unique_ptr<IRenderBatch>> s_Batches;

		static std::unordered_map<MeshBatchKey, MeshBatch*, MeshBatchKeyHasher> s_MeshBatchLookup;
		static std::unordered_map<ParticleBatchKey, ParticleBatch*, ParticleBatchKeyHasher> s_ParticleBatchLookup;
		static std::unordered_map<UIBatchKey, UIBatch*, UIBatchKeyHasher> s_UIBatchLookup;

		static int s_ViewportWidth;
		static int s_ViewportHeight;
	};
}
