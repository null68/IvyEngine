#pragma once

#include <memory>
#include <vector>
#include <glm.hpp>

#include "./ScriptComponent.h"
#include "../../Particles/Particle.h"
#include "../../Graphics/Render/RenderQueue.h"
#include "../../Graphics/Shader/ShaderProgram.h"
#include "../../Graphics/Texture/Texture.h"

namespace Engine {
	// particle emitter component doesnt need to be extended
	class ParticleEmitterComponent final : public ScriptComponent {
	public:
		float EmissionRate = 20.0f;  
		int MaxParticles = 500;
		bool Looping = true;
		bool Playing = true;
		float SpawnRadius = 0.0f; 

		float MinLifetime = 1.0f;
		float MaxLifetime = 2.0f;
		glm::vec3 MinVelocity{ -1.0f, 2.0f, -1.0f };
		glm::vec3 MaxVelocity{ 1.0f, 4.0f, 1.0f };
		float MinRotationSpeed = 0.0f;
		float MaxRotationSpeed = 0.0f;

		float StartSize = 0.3f;
		float EndSize = 0.0f;
		glm::vec4 StartColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 EndColor{ 1.0f, 1.0f, 1.0f, 0.0f };

		glm::vec3 Gravity{ 0.0f, -1.0f, 0.0f };
		float Drag = 0.0f;

		RenderQueue Queue = RenderQueue::Particles;
		bool LocalSpace = false;

		ParticleEmitterComponent() = default;

		void OnCreate() override;
		void OnUpdate(float deltaTime) override;

		void Play() { Playing = true; }
		void Stop() { Playing = false; }
		void Burst(int count);

		void SetShader(std::shared_ptr<ShaderProgram> shader) { m_Shader = std::move(shader); }
		void SetTexture(std::shared_ptr<Texture> texture) { m_Texture = std::move(texture); }

		ShaderProgram* GetShader() const;
		Texture* GetTexture() const { return m_Texture.get(); }

		const std::vector<Particle>& GetParticles() const { return m_Particles; }
		int GetAliveCount() const { return static_cast<int>(m_Particles.size()); }

	private:
		void SpawnParticle(const glm::vec3& originWorld);
		glm::vec3 GetOriginWorld() const;

		std::vector<Particle> m_Particles;
		float m_EmitAccumulator = 0.0f;
		glm::vec3 m_LastOrigin{ 0.0f };

		std::shared_ptr<ShaderProgram> m_Shader;
		std::shared_ptr<Texture> m_Texture;
	};
}
