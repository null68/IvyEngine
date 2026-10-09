#pragma once

#include <glm.hpp>

namespace Engine {
	struct Particle {
		glm::vec3 Position{ 0.0f };
		glm::vec3 Velocity{ 0.0f };

		float Rotation = 0.0f;
		float RotationSpeed = 0.0f;

		float Size = 1.0f;
		glm::vec4 Color{ 1.0f };

		float Age = 0.0f;
		float Lifetime = 1.0f;

		bool IsAlive() const { return Age < Lifetime; }
		float LifeT() const { return Lifetime > 0.0f ? glm::clamp(Age / Lifetime, 0.0f, 1.0f) : 1.0f; }
	};
}
