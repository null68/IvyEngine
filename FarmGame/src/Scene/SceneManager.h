#pragma once

#include <unordered_map>
#include <type_traits>
#include <utility>
#include "Scene.h"

namespace Engine {
	class SceneManager {
	private:
		std::string m_MainSceneName;
		Scene* m_ActiveScene = nullptr;
		Scene* RegisterScene(std::unique_ptr<Scene> scene);
	public:
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;

		static SceneManager& GetInstance();

		template <typename T, typename... Args>
		T* CreateScene(Args&&... args) {
			static_assert(std::is_base_of<Scene, T>::value, "T must dervie from Engine::Scene");

			auto scene = std::make_unique<T>(std::forward<Args>(args)...);

			Scene* registeredScene = RegisterScene(std::move(scene));

			if (!registeredScene) {
				return nullptr;
			}

			return dynamic_cast<T*>(registeredScene);
		}
		bool SetMainScene(const char* name);
		Scene* GetMainScene();
		Scene* LoadMainScene();

		Scene* GetScene(const char* name);
		Scene* LoadScene(const char* name);
		Scene* GetActiveScene() const { return m_ActiveScene; }
		void UnloadScene(const char* name);
		void DeleteScene(const char* name);
	};
}

#define REGISTER_SCENE(Scene, IsMainScene, ...) \
	const bool Scene##_registered = []() { \
		auto& instance = Engine::SceneManager::GetInstance(); \
		auto* scene = instance.CreateScene<Scene>(__VA_ARGS__); \
		if(!scene) { \
			return false; \
		} \
		if(IsMainScene) { \
			return instance.SetMainScene(scene->m_Name); \
		} \
		return true; \
	}(); 
