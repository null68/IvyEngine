#include <memory>
#include <map>
#include "../Texture/Texture.h"
#include <glad/glad.h>


namespace Engine {
	class Glyph {
	public:
		unsigned int m_TextureId;
		int m_Height;
		int m_Width;
		int m_BearingX;
		int m_BearingY;
		unsigned int m_Advance;
	};

	class Font {
	public:
		std::map<unsigned char, Glyph> m_Glyphs;
		int m_PixelSize = 0;

		Font() = default;
		~Font() {
			for (auto& g : m_Glyphs) {
				if (g.second.m_TextureId != 0) {
					glDeleteTextures(1, &g.second.m_TextureId);
				}
			}
		};

		Font(const Font& other) = delete;
		Font& operator=(const Font& other) = delete;
	};
}