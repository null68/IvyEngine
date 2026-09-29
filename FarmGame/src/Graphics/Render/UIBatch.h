#pragma once

#include "IRenderBatch.h"
#include "../Shader/ShaderProgram.h"
#include "../Texture/Texture.h"

#include <glm.hpp>
#include <vector>

namespace Engine {
	class UIBatch : public IRenderBatch {
	public:
		struct InstanceData {
			glm::vec2 Position; 
			glm::vec2 Size;     
			glm::vec4 Color;
			glm::vec4 UVRect;   
			float Rotation;  
		};

		UIBatch(ShaderProgram* shader, const Texture* texture, int zOrder);
		~UIBatch() override;

		void Begin() override;
		void End() override;
		void Flush(const glm::mat4& view, const glm::mat4& projection) override;

		bool HasContent() const override { return !m_Instances.empty(); }
		bool IsFull() const override;
		RenderQueue GetQueue() const override { return RenderQueue::UI; }
		int GetSortOrder() const override { return m_ZOrder; }

		void Submit(const InstanceData& instance);

	private:
		void UploadInstanceBuffer();

		ShaderProgram* m_Shader;
		const Texture* m_Texture;
		int m_ZOrder;

		std::vector<InstanceData> m_Instances;

		static constexpr size_t MAX_INSTANCES = 512;

		unsigned int m_VAO = 0;
		unsigned int m_InstanceVBO = 0;
	};
}
