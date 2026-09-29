#include "QuadGeometry.h"
#include <glad/glad.h>
#include <cstddef>

namespace Engine {
	namespace {
		struct QuadVertex {
			float x, y;
			float u, v;
		};
	}

	QuadGeometry& QuadGeometry::Get() {
		static QuadGeometry instance;
		return instance;
	}

	QuadGeometry::QuadGeometry() {
		QuadVertex vertices[4] = {
			{ -0.5f, -0.5f, 0.0f, 0.0f },
			{  0.5f, -0.5f, 1.0f, 0.0f },
			{  0.5f,  0.5f, 1.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	QuadGeometry::~QuadGeometry() {
		Release();
	}

	void QuadGeometry::Release() {
		if (m_IBO) { glDeleteBuffers(1, &m_IBO); m_IBO = 0; }
		if (m_VBO) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
	}

	void QuadGeometry::Shutdown() {
		Get().Release();
	}

	void QuadGeometry::ConfigureBaseAttributes() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, x));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, u));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	}
}
