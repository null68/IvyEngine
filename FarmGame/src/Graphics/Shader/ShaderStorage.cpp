#include "ShaderStorage.h"
#include "../../Assets/AssetManager.h"

namespace Engine {
	ShaderStorage& ShaderStorage::GetInstance()
	{
		static ShaderStorage instance;
		return instance;
	}

	std::shared_ptr<ShaderProgram> ShaderStorage::Load(std::string name, const char* vertexShaderFilePath, const char* fragmentShaderFilePath) {
		const char* vertexShaderSource = AssetManager::LoadShaderFile(vertexShaderFilePath);
		const char* fragmentShaderSource = AssetManager::LoadShaderFile(fragmentShaderFilePath);
		
		if (Exists(name))
			return m_Shaders[name];

		auto shader = std::make_shared<ShaderProgram>();
		shader->AttachVertex(vertexShaderSource);
		shader->AttachFragment(fragmentShaderSource);
		shader->Link();
		m_Shaders[name] = shader;
		return shader;
	}
	std::shared_ptr<ShaderProgram> ShaderStorage::Get(std::string name)
	{
		if (Exists(name))
			return m_Shaders[name];

		throw std::runtime_error("Shader with name " + name + " does not exist in storage.");
	}
	bool ShaderStorage::Exists(const std::string& name) const {
		return m_Shaders.find(name) != m_Shaders.end();
	}
	void ShaderStorage::Shutdown() {
		m_Shaders.clear();
	}
}