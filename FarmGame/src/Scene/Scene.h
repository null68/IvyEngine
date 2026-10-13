#pragma once

#include "../ECS/EntityManager.h"
#include "../ECS/Components/CameraComponent.h"

namespace Engine {
	class Scene {
	public:
		const char* m_Name = nullptr;
		bool m_IsLoaded = false;
		bool m_IsInitialized = false;
		EntityManager* m_EntityManager = nullptr;

		Scene(const char* name);
		virtual ~Scene();
			
		virtual void Initialize() = 0;
		virtual CameraComponent* GetMainCamera() const final;

		bool operator==(Scene& scene) {
			if (strcmp(m_Name, scene.m_Name) == 0)
				return true;
			return false;
		}

		bool operator!=(Scene& scene) {
			return !(*this == scene);
		}
	};
}