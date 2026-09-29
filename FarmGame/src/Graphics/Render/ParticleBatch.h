#pragma once

#include "IRenderBatch.h"
#include "../Shader/ShaderProgram.h"
#include "../Texture/Texture.h"
#include "../../Particles/Particle.h"

#include <glm.hpp>
#include <vector>

namespace Engine {
	class ParticleBatch : public IRenderBatch {
	public:
		ParticleBatch(ShaderProgram* shader, const Texture* texture, RenderQueue queue);
		~ParticleBatch() override;

		void Begin() override;
		void End() override;
		void Flush(const glm::mat4& view, const glm::mat4& projection) override;

		bool HasContent() const override { return !m_Instances.empty(); }
		bool IsFull() const override;
		RenderQueue GetQueue() const override { return m_Queue; }

		void Submit(const Particle& particle);

	private:
		struct InstanceData {
			glm::vec4 PositionSize; 
			glm::vec4 Color;
			float Rotation;
		};

		void UploadInstanceBuffer();

		ShaderProgram* m_Shader;
		const Texture* m_Texture;
		RenderQueue m_Queue;

		std::vector<InstanceData> m_Instances;

		static constexpr size_t MAX_INSTANCES = 4096;

		unsigned int m_VAO = 0;
		unsigned int m_InstanceVBO = 0;
	};
}
