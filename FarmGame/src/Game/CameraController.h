#pragma once

#include <glm.hpp>
#include <GLFW/glfw3.h>

#include "../ECS/Entity.h"
#include "../ECS/Components/ScriptComponent.h"
#include "../ECS/Components/CameraComponent.h"
#include "../Core/Input.h"

namespace Game {
	class CameraController : public Engine::ScriptComponent {
	public:
		explicit CameraController(glm::vec3 startPosition, float speed = 5.0f, float sensitivity = 0.1f)
			: m_Position(startPosition), m_Speed(speed), m_Sensitivity(sensitivity) {
		}

		void OnUpdate(float deltaTime) override {
			auto* camera = entity->GetComponent<Engine::CameraComponent>();
			if (!camera) return;

			Engine::Input* input = Engine::Input::Get();
			if (!input) return;
			input->SetMouseVisible(false);
			double xpos, ypos;
			input->GetMousePosition(xpos, ypos);
			glm::vec2 mouse{ xpos, ypos };
			if (m_FirstMouse) {
				m_LastMouse = mouse;
				m_FirstMouse = false;
			}

			glm::vec2 delta = (mouse - m_LastMouse) * m_Sensitivity;
			m_LastMouse = mouse;

			m_Yaw += delta.x;
			m_Pitch = glm::clamp(m_Pitch - delta.y, -89.0f, 89.0f);

			glm::vec3 front;
			front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
			front.y = glm::sin(glm::radians(m_Pitch));
			front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
			front = glm::normalize(front);

			glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

			float velocity = m_Speed * deltaTime;
			if (input->IsKeyPressed(GLFW_KEY_W)) m_Position += front * velocity;
			if (input->IsKeyPressed(GLFW_KEY_S)) m_Position -= front * velocity;
			if (input->IsKeyPressed(GLFW_KEY_A)) m_Position -= right * velocity;
			if (input->IsKeyPressed(GLFW_KEY_D)) m_Position += right * velocity;

			camera->SetPosition(m_Position);
			camera->LookAt(m_Position + front);
		}

	private:
		glm::vec3 m_Position;
		float m_Speed;
		float m_Sensitivity;
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		glm::vec2 m_LastMouse{ 0.0f };
		bool m_FirstMouse = true;
	};
}