#include "ParticleEmitterComponent.h"

#include <gtc/random.hpp>

#include "../Entity.h"
#include "./TransformComponent.h"
#include "../../Graphics/Shader/ShaderStorage.h"

namespace Engine {
	glm::vec3 ParticleEmitterComponent::GetOriginWorld() const {
		if (entity) {
			if (auto* transform = entity->GetComponent<TransformComponent>()) {
				return transform->Position;
			}
		}
		return glm::vec3(0.0f);
	}

	void ParticleEmitterComponent::OnCreate() {
		m_Particles.reserve(static_cast<size_t>(MaxParticles));
		m_LastOrigin = GetOriginWorld();
	}

	void ParticleEmitterComponent::SpawnParticle(const glm::vec3& originWorld) {
		if (static_cast<int>(m_Particles.size()) >= MaxParticles)
			return;

		Particle p;
		p.Lifetime = glm::linearRand(MinLifetime, MaxLifetime);
		p.Velocity = glm::linearRand(MinVelocity, MaxVelocity);
		p.RotationSpeed = glm::linearRand(MinRotationSpeed, MaxRotationSpeed);

		glm::vec3 offset(0.0f);
		if (SpawnRadius > 0.0f) {
			offset = glm::sphericalRand(glm::linearRand(0.0f, SpawnRadius));
		}

		p.Position = originWorld + offset;
		p.Rotation = 0.0f;
		p.Size = StartSize;
		p.Color = StartColor;
		p.Age = 0.0f;

		m_Particles.push_back(p);
	}

	void ParticleEmitterComponent::Burst(int count) {
		glm::vec3 originWorld = GetOriginWorld();
		for (int i = 0; i < count; i++) {
			SpawnParticle(originWorld);
		}
	}

	void ParticleEmitterComponent::OnUpdate(float deltaTime) {
		glm::vec3 originWorld = GetOriginWorld();
		glm::vec3 frameDelta = originWorld - m_LastOrigin;
		m_LastOrigin = originWorld;
		size_t writeIndex = 0;
		for (size_t i = 0; i < m_Particles.size(); i++) {
			Particle p = m_Particles[i];
			p.Age += deltaTime;

			if (p.IsAlive()) {
				if (LocalSpace)
					p.Position += frameDelta;

				p.Velocity += Gravity * deltaTime;
				if (Drag > 0.0f)
					p.Velocity *= glm::max(0.0f, 1.0f - Drag * deltaTime);

				p.Position += p.Velocity * deltaTime;
				p.Rotation += p.RotationSpeed * deltaTime;

				float t = p.LifeT();
				p.Size = glm::mix(StartSize, EndSize, t);
				p.Color = glm::mix(StartColor, EndColor, t);

				m_Particles[writeIndex++] = p;
			}
		}
		m_Particles.resize(writeIndex);

		if (Playing && Looping && EmissionRate > 0.0f) {
			m_EmitAccumulator += EmissionRate * deltaTime;
			while (m_EmitAccumulator >= 1.0f && static_cast<int>(m_Particles.size()) < MaxParticles) {
				SpawnParticle(originWorld);
				m_EmitAccumulator -= 1.0f;
			}
		}
	}

	ShaderProgram* ParticleEmitterComponent::GetShader() const {
		if (m_Shader)
			return m_Shader.get();
		return ShaderStorage::GetInstance().Get("ParticleShader").get();
	}
}
