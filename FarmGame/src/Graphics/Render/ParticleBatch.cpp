#include "ParticleBatch.h"
#include "../Geometry/QuadGeometry.h"

#include <glad/glad.h>
#include <cstddef>

namespace Engine {
	ParticleBatch::ParticleBatch(ShaderProgram* shader, const Texture* texture, RenderQueue queue)
		: m_Shader(shader), m_Texture(texture), m_Queue(queue) {

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_InstanceVBO);

		glBindVertexArray(m_VAO);
		QuadGeometry::Get().ConfigureBaseAttributes(); // locations 0 (position) and 1 (uv)

		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, PositionSize));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Color));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, Rotation));
		glVertexAttribDivisor(4, 1);

		glBindVertexArray(0);
	}

	ParticleBatch::~ParticleBatch() {
		if (m_InstanceVBO) glDeleteBuffers(1, &m_InstanceVBO);
		if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
	}

	void ParticleBatch::Begin() {
		m_Instances.clear();
		m_Shader->Bind();
	}

	void ParticleBatch::Submit(const Particle& particle) {
		InstanceData data;
		data.PositionSize = glm::vec4(particle.Position, particle.Size);
		data.Color = particle.Color;
		data.Rotation = particle.Rotation;
		m_Instances.push_back(data);
	}

	void ParticleBatch::UploadInstanceBuffer() {
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
		glBufferData(GL_ARRAY_BUFFER, m_Instances.size() * sizeof(InstanceData), m_Instances.data(), GL_DYNAMIC_DRAW);
	}

	void ParticleBatch::End() {
		if (!HasContent())
			return;
		UploadInstanceBuffer();
	}

	void ParticleBatch::Flush(const glm::mat4& view, const glm::mat4& projection) {
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

	bool ParticleBatch::IsFull() const {
		return m_Instances.size() >= MAX_INSTANCES;
	}
}
