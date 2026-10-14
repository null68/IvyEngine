#include <memory>
#include "../Texture/Texture.h"
#include <unordered_map>

namespace Engine {

	struct Glyph {
		float m_U0 = 0.0f;
		float m_V0 = 0.0f;
		float m_U1 = 0.0f;
		float m_V1 = 0.0f;

		int m_Width = 0;
		int m_Height = 0;

		int m_BearingX = 0;
		int m_BearingY = 0;

		unsigned int m_Advance = 0;
	};

	struct Font {
		std::unique_ptr<Texture> m_Texture;
		std::unordered_map<unsigned char, Glyph> m_Glyphs;
		int m_PixelSize = 48;
	};

}