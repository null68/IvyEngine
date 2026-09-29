#pragma once

#include <memory>
#include <glm.hpp>

#include "../Component.h"
#include "../../Graphics/Shader/ShaderProgram.h"
#include "../../Graphics/Texture/Texture.h"

namespace Engine {
	class UIElementComponent : public Component {
	public:
		glm::vec2 Position{ 0.0f, 0.0f };
		glm::vec2 Size{ 100.0f, 100.0f };
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Rotation = 0.0f;
		glm::vec4 UVRect{ 0.0f, 0.0f, 1.0f, 1.0f };

		bool Visible = true;
		int ZOrder = 0;

		std::shared_ptr<Texture> texture;
		std::shared_ptr<ShaderProgram> shader; 
	};
}
