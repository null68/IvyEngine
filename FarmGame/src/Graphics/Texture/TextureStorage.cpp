#include "TextureStorage.h"
#include <stdexcept>

namespace Engine {
	TextureStorage& TextureStorage::GetInstance()
	{
		static TextureStorage instance;
		return instance;
	}

	std::shared_ptr<Texture> TextureStorage::Load(const std::string& name, const std::string& filePath, bool flipVertically) {
		if (Exists(name))
			return m_Textures[name];

		auto texture = std::make_shared<Texture>(filePath, flipVertically);
		m_Textures[name] = texture;
		return texture;
	}

	std::shared_ptr<Texture> TextureStorage::Load(const std::string& name, const unsigned char* data, int width, int height, int channels) {
		if (Exists(name))
			return m_Textures[name];

		auto texture = std::make_shared<Texture>(
			data,
			width,
			height,
			channels
		);

		m_Textures[name] = texture;
		return texture;
	}


	std::shared_ptr<Texture> TextureStorage::Get(const std::string& name)
	{
		if (Exists(name))
			return m_Textures[name];

		throw std::runtime_error("Texture with name " + name + " does not exist in storage.");
	}

	bool TextureStorage::Exists(const std::string& name) const {
		return m_Textures.find(name) != m_Textures.end();
	}
}
