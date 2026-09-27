#include "Demo.h"

#include "../Scene/SceneManager.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/MeshComponent.h"
#include "../ECS/Components/MaterialComponent.h"
#include "../ECS/Components/CameraComponent.h"
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

		Entity* cameraEntity = entities->CreateEntity();
		auto& camera = cameraEntity->AddComponent<CameraComponent>();
		camera.Position = glm::vec3(0.0f, 2.0f, 5.0f);

		auto& cameraTransform = cameraEntity->AddComponent<TransformComponent>();
		cameraEntity->AddComponent<CameraController>(glm::vec3(0.0f, 2.0f, 5.0f));
	}
}