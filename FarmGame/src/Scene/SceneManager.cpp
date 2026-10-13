#include "SceneManager.h"

namespace Engine {
	SceneManager& SceneManager::GetInstance() {
		static SceneManager instance;
		return instance;
	}
	
	Scene* SceneManager::RegisterScene(std::unique_ptr<Scene> scene) {
		if (!scene || !scene->m_Name || !*scene->m_Name) {
			std::cerr << "Cannot register scene with an invalid name" << std::endl;
			return nullptr;
		}

		auto it = m_Scenes.find(scene->m_Name);
		if (it != m_Scenes.end()) {
			std::cerr << "Scene with name " << scene->m_Name << " already exists." << std::endl;
			return nullptr;
		}

		Scene* registeredScene = scene.get();
		m_Scenes.emplace(scene->m_Name, std::move(scene));

		return registeredScene;
	}

	Scene* SceneManager::GetScene(const char* name) {
		if (!name) {
			return nullptr;
		}

		auto it = m_Scenes.find(name);
		if (it != m_Scenes.end()) {
			return it->second.get();
		}

		return nullptr;
	}


	bool SceneManager::SetMainScene(const char* name) {
		if (!name || !GetScene(name)) {
			std::cerr << "Scene does not exist. Cannot set main scene." << std::endl;
			return false;
		}

		m_MainSceneName = name;
		return true;
	}

	Scene* SceneManager::GetMainScene() {
		if (m_MainSceneName.empty()) {
			return nullptr;
		}
		
		return GetScene(m_MainSceneName.c_str());
	}

	Scene* SceneManager::LoadMainScene() {
		Scene* scene = GetMainScene();
	
		if (!scene) {
			std::cerr << "Main scene has not been set." << std::endl;;
			return nullptr;
		}
		
		return LoadScene(scene->m_Name);
	}


	Scene* SceneManager::LoadScene(const char* name) {
		auto scene = GetScene(name);

		if (!scene) {
			std::cerr << "Scene with name " << name << " does not exist. Cannot load scene." << std::endl;
			return nullptr;
		}

		if (!scene->m_IsInitialized) {
			scene->Initialize();
			scene->m_IsInitialized = true;
		}

		if (m_ActiveScene) {
			m_ActiveScene->m_IsLoaded = false;
		}

		m_ActiveScene = scene;
		m_ActiveScene->m_IsLoaded = true;

		return scene;
	}

	void SceneManager::UnloadScene(const char* name) {
		auto it = m_Scenes.find(name);

		if (it != m_Scenes.end()) {

			it->second->m_IsLoaded = false;
			if (m_ActiveScene == it->second.get()) {
				m_ActiveScene = nullptr;
			}
		}
		else {
			std::cerr << "Scene with name " << name << " does not exist. Cannot unload scene." << std::endl;
		}
	}
	
	void SceneManager::DeleteScene(const char* name) {
		auto it = m_Scenes.find(name);

		if (it != m_Scenes.end()) {
			return;
		}

		Scene* scene = it->second.get();

		if (m_ActiveScene == scene) {
			m_ActiveScene = nullptr;
		}

		if (m_MainSceneName == name) {
			m_MainSceneName.clear();
		}

		m_Scenes.erase(it);
	}
}