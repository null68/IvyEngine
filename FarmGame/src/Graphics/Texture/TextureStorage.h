#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "Texture.h"

namespace Engine {
	class TextureStorage {
	public:
		static TextureStorage& GetInstance();

		std::shared_ptr<Texture> Load(const std::string& name, const std::string& filePath, bool flipVertically = true);
		std::shared_ptr<Texture> Load(const std::string& name, const unsigned char* data, int width, int height, int channels);
		std::shared_ptr<Texture> Get(const std::string& name);
		bool Exists(const std::string& name) const;

		void Shutdown();
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	};
}
