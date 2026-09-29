#pragma once

#include "IRenderBatch.h"
#include "../Mesh/Mesh.h"
#include "../Shader/ShaderProgram.h"

#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/MaterialComponent.h"

namespace Engine {
	// GPU-instanced batch of one mesh drawn with one material/shader, submitted as a
	// list of per-instance model matrices. This is the original renderer's only batch
	// type; it now just implements IRenderBatch so it can live alongside ParticleBatch,
	// UIBatch, etc. in the same render queue.
	class MeshBatch : public IRenderBatch {
	public:
		MeshBatch(ShaderProgram* shader);
		~MeshBatch() override;

		void Begin() override;
		void End() override;
		void Flush(const glm::mat4& view, const glm::mat4& projection) override;

		bool HasContent() const override { return m_Mesh != nullptr && !m_Transforms.empty(); }
		bool IsFull() const override;
		RenderQueue GetQueue() const override { return m_Material->queue; }

		void Submit(const Mesh& mesh, const TransformComponent& transform, const MaterialComponent& material);

		const Mesh* GetMesh() const { return m_Mesh; }
		const MaterialComponent* GetMaterial() const { return m_Material; }
		inline ShaderProgram* GetShader() const { return m_Shader; }
	private:
		void UploadInstanceBuffer();

		ShaderProgram* m_Shader;
		const Mesh* m_Mesh = nullptr;
		const MaterialComponent* m_Material = nullptr;

		std::vector<glm::mat4> m_Transforms;

		static constexpr size_t MAX_INSTANCES = 1024;

		unsigned int m_InstanceVBO = 0;
	};
}
