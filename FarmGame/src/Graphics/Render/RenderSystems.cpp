#include "RenderSystems.h"
#include "Renderer.h"
#include "../Shader/ShaderStorage.h"

#include "../../ECS/EntityManager.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/MeshComponent.h"
#include "../../ECS/Components/MaterialComponent.h"
#include "../../ECS/Components/UIElementComponent.h"
#include "../../ECS/Components/ParticleEmitterComponent.h"

namespace Engine {
	namespace RenderSystems {

		void SubmitMeshes(EntityManager& entities) {
			for (Entity* entity : entities.GetEntities()) {
				auto [transform, meshComp, material] =
					entity->GetComponents<TransformComponent, MeshComponent, MaterialComponent>();

				if (transform && meshComp && meshComp->mesh && material) {
					Renderer::Submit(*meshComp->mesh, *transform, *material);
				}
			}
		}

		void SubmitParticles(EntityManager& entities) {
			for (Entity* entity : entities.GetEntities()) {
				auto* emitter = entity->GetComponent<ParticleEmitterComponent>();
				if (!emitter)
					continue;

				ShaderProgram* shader = emitter->GetShader();
				const Texture* texture = emitter->GetTexture();

				for (const Particle& particle : emitter->GetParticles()) {
					Renderer::SubmitParticle(particle, shader, texture, emitter->Queue);
				}
			}
		}

		void SubmitUI(EntityManager& entities) {
			for (Entity* entity : entities.GetEntities()) {
				auto* ui = entity->GetComponent<UIElementComponent>();
				if (!ui || !ui->Visible)
					continue;

				ShaderProgram* shader = ui->shader
					? ui->shader.get()
					: ShaderStorage::GetInstance().Get("UIShader").get();

				UIBatch::InstanceData instance;
				instance.Position = ui->Position;
				instance.Size = ui->Size;
				instance.Color = ui->Color;
				instance.UVRect = ui->UVRect;
				instance.Rotation = ui->Rotation;

				Renderer::SubmitUI(instance, shader, ui->texture.get(), ui->ZOrder);
			}
		}

	}
}
