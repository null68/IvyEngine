#pragma once

#include <unordered_map>
#include <string>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

namespace Engine {
	class ShaderProgram {
	public:
		ShaderProgram();
		~ShaderProgram();
		void AttachVertex(const char* vertexSource);
		void AttachFragment(const char* fragmentSource);
		void Link();
		void Bind() const;
		void Unbind() const;
		unsigned int getId() const { return m_Id; };
		int GetUniformLocation(const char* name) const; 
		void SetUniform1i(const char* name, int value) const;
		void SetUniform1f(const char* name, float value) const;
		void SetUniformVec2f(const char* name, const glm::vec2& vec) const;
		void SetUniformVec3f(const char* name, const glm::vec3& vec) const;
		void SetUniformVec4f(const char* name, const glm::vec4& vec) const;
		void SetUniformMat4f(const char* name, const glm::mat4& mat) const;
		void SetUniform1b(const char* name, bool value) const;
	private:
		unsigned int m_Id;
		unsigned int m_VertexShaderId;
		unsigned int m_FragmentShaderId;
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
	};
}