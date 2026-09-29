#include "UIBatch.h"
#include "../Geometry/QuadGeometry.h"

#include <glad/glad.h>
#include <cstddef>

namespace Engine {
	UIBatch::UIBatch(ShaderProgram* shader, const Texture* texture, int zOrder)
		: m_Shader(shader), m_Texture(texture), m_ZOrder(zOrder) {

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_InstanceVBO);

		glBindVertexArray(m_VAO);
		QuadGeometry::Get().ConfigureBaseAttributes();

		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Position));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Size));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Color));
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, UVRect));
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Rotation));
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	UIBatch::~UIBatch() {
		if (m_InstanceVBO) glDeleteBuffers(1, &m_InstanceVBO);
		if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
	}

	void UIBatch::Begin() {
		m_Instances.clear();
		m_Shader->Bind();
	}

	void UIBatch::Submit(const InstanceData& instance) {
		m_Instances.push_back(instance);
	}

	void UIBatch::UploadInstanceBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
		glBufferData(GL_ARRAY_BUFFER, m_Instances.size() * sizeof(InstanceData), m_Instances.data(), GL_DYNAMIC_DRAW);
	}

	void UIBatch::End() {
		if (!HasContent())
			return;
		UploadInstanceBuffer();
	}

	void UIBatch::Flush(const glm::mat4& view, const glm::mat4& projection) {
		if (!HasContent())
			return;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_View", view);
		m_Shader->SetUniformMat4f("u_Projection", projection);

		bool hasTexture = m_Texture != nullptr && m_Texture->IsValid();
		m_Shader->SetUniform1b("u_UseTexture", hasTexture);
		if (hasTexture) {
			m_Texture->Bind(0);
			m_Shader->SetUniform1i("u_Texture", 0);
		}

		glBindVertexArray(m_VAO);
		glDrawElementsInstanced(
			GL_TRIANGLES,
			QuadGeometry::Get().GetIndexCount(),
			GL_UNSIGNED_INT,
			nullptr,
			static_cast<GLsizei>(m_Instances.size())
		);
		glBindVertexArray(0);

		m_Instances.clear();
	}

	bool UIBatch::IsFull() const {
		return m_Instances.size() >= MAX_INSTANCES;
	}
}
