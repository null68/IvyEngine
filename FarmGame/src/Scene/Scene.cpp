#include "Scene.h"


namespace Engine {
	Scene::Scene(const char* name) : m_Name(name), m_EntityManager(new EntityManager()) {
	}
	Scene::~Scene() {
		delete m_EntityManager;
		m_EntityManager = nullptr;
	}

	CameraComponent* Scene::GetMainCamera() const {
		for (Entity* e : m_EntityManager->GetEntities()) {
			if (e->HasComponent<CameraComponent>()) {
				return e->GetComponent<CameraComponent>();
			}
		}
		return nullptr;
	}

}