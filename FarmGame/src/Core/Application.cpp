#include <iostream>

#include "Application.h"
#include "../Graphics/Render/Renderer.h"
#include "../Graphics/Render/RenderSystems.h"
#include "../Scene/SceneManager.h"
#include "../ECS/Components/CameraComponent.h"

#include "../Game/Demo.h"

namespace Engine {
	Application::Application() {
		Window = std::make_unique<Engine::Window>(800, 600, "Farm Game");
		Input = std::make_unique<Engine::Input>(Window->GetNativeWindow());
		Renderer::Init();

		// citav gameplay se pokrece kroz scene koje se naprave u Game/ dir-u, zatim se kreiraju ovdje i pokrenu sa SceneManager::GetInstance().LoadScene("")
		Game::CreateDemoScene();
		SceneManager::GetInstance().LoadScene("Demo");
	}
	void Application::Run() {
		while (!Window->ShouldClose()) {
			Window->PollEvents();
			Time::Update();

			if (Input->IsKeyPressed(GLFW_KEY_ESCAPE)) { 
				std::cout << "Escape key pressed. Exiting application." << std::endl;
				break;
			}

			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene) {
				scene->m_EntityManager->Update(Time::DeltaTime());

				CameraComponent* camera = scene->GetMainCamera();
				if (camera) {
					Renderer::BeginScene(*camera);

					RenderSystems::SubmitMeshes(*scene->m_EntityManager);
					RenderSystems::SubmitParticles(*scene->m_EntityManager);
					RenderSystems::SubmitUI(*scene->m_EntityManager);

					Renderer::EndScene();
				}
			}

			Window->SwapBuffers();
		}

		Renderer::Shutdown();
	}

}