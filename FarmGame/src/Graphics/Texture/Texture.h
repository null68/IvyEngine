#pragma once
#pragma once

#include <string>

namespace Engine {
	class Texture {
	public:
		explicit Texture(const std::string& path, bool flipVertically = true);
		explicit Texture(const unsigned char* data, int width, int height, int channels);
		~Texture();
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		unsigned int GetId() const { return m_Id; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		bool IsValid() const { return m_Id != 0; }

	private:
		unsigned int m_Id = 0;
		int m_Width = 0;
		int m_Height = 0;
		int m_Channels = 0;
		std::string m_Path;
	};
}
