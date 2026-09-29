#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include "ShaderProgram.h"

namespace Engine {
	class ShaderStorage {
	public:
		static ShaderStorage& GetInstance();

		std::shared_ptr<ShaderProgram> Load(std::string name, const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
		std::shared_ptr<ShaderProgram> Get(std::string name);
		bool Exists(const std::string& name) const;

		void Shutdown();
	private:
		std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_Shaders;
	};
}