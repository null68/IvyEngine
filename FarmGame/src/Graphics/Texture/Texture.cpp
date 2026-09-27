#include "Texture.h"
#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"

namespace Engine {
	Texture::Texture(const std::string& path, bool flipVertically)
		: m_Path(path) {
		stbi_set_flip_vertically_on_load(flipVertically);

		unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
		if (!data) {
			std::cerr << "Failed to load texture: " << path << " (" << stbi_failure_reason() << ")" << std::endl;
			return;
		}

		GLenum internalFormat = GL_RGB8;
		GLenum dataFormat = GL_RGB;
		if (m_Channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (m_Channels == 1) {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}
		else if (m_Channels != 3) {
			std::cerr << "Texture " << path << " has unsupported channel count: " << m_Channels << std::endl;
			stbi_image_free(data);
			return;
		}

		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}

	Texture::Texture(const unsigned char* data, int width, int height, int channels) {
		m_Width = width;
		m_Height = height;
		m_Channels = channels;
		GLenum format = GL_RGB;
		if (channels == 4) {
			format = GL_RGBA;
		}
		else if (channels == 1) {
			format = GL_RED;
		}
		else if (channels == 2) {
			format == GL_RG;
		}

		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture() {
		if (m_Id != 0) {
			glDeleteTextures(1, &m_Id);
		}
	}

	void Texture::Bind(unsigned int slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_Id);
	}

	void Texture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
