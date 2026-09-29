#include "Demo.h"

#include "../Scene/SceneManager.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/MeshComponent.h"
#include "../ECS/Components/MaterialComponent.h"
#include "../ECS/Components/CameraComponent.h"
#include "../ECS/Components/UIElementComponent.h"
#include "../ECS/Components/ParticleEmitterComponent.h"
#include "../Graphics/Shader/ShaderStorage.h"
#include "../Model/Model.h"
#include "CameraController.h"

namespace Game {
	void CreateDemoScene() {
		using namespace Engine;

		Scene* scene = SceneManager::GetInstance().CreateScene("Demo");
		EntityManager* entities = scene->m_EntityManager;

		Model stonesModel("assets/models/stone.glb");
		Entity* stones = entities->CreateEntity();

		auto& transform = stones->AddComponent<TransformComponent>();
		transform.Position = glm::vec3(0.0f);
		transform.Rotation = glm::vec3(0.0f);
		transform.Scale = glm::vec3(1.0f);

		auto& mesh = stones->AddComponent<MeshComponent>();
		mesh.mesh = stonesModel.GetMesh("stone_small");

		auto& material = stones->AddComponent<MaterialComponent>();
		material.shader = ShaderStorage::GetInstance().Get("BasicShader");
		auto texture = stonesModel.GetMeshTexture("stone_small");

		material.texture = texture;
		material.textureSlot = 0;


		Entity* dust = entities->CreateEntity();
		dust->AddComponent<TransformComponent>().Position = glm::vec3(0.0f, 0.2f, 0.0f);

		auto& emitter = dust->AddComponent<ParticleEmitterComponent>();
		emitter.EmissionRate = 12.0f;
		emitter.MaxParticles = 100;
		emitter.MinLifetime = 1.0f;
		emitter.MaxLifetime = 2.0f;
		emitter.MinVelocity = glm::vec3(-0.15f, 0.3f, -0.15f);
		emitter.MaxVelocity = glm::vec3(0.15f, 0.6f, 0.15f);
		emitter.Gravity = glm::vec3(0.0f, 0.05f, 0.0f); 
		emitter.SpawnRadius = 0.3f;
		emitter.StartSize = 0.08f;
		emitter.EndSize = 0.25f;
		emitter.StartColor = glm::vec4(0.85f, 0.8f, 0.7f, 0.5f);
		emitter.EndColor = glm::vec4(0.85f, 0.8f, 0.7f, 0.0f);
	
		Entity* hudPanel = entities->CreateEntity();
		auto& panel = hudPanel->AddComponent<UIElementComponent>();
		panel.Position = glm::vec2(16.0f, 16.0f);
		panel.Size = glm::vec2(160.0f, 48.0f);
		panel.Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
		panel.ZOrder = 0;


		Entity* cameraEntity = entities->CreateEntity();
		auto& camera = cameraEntity->AddComponent<CameraComponent>();
		camera.Position = glm::vec3(0.0f, 2.0f, 5.0f);

		auto& cameraTransform = cameraEntity->AddComponent<TransformComponent>();
		cameraEntity->AddComponent<CameraController>(glm::vec3(0.0f, 2.0f, 5.0f));
	}
}