#include "Input.h"

namespace Engine {
	Input* Input::s_Instance = nullptr;

	Input::Input(GLFWwindow* window)
		: m_Window(window) {
		s_Instance = this;
	}
	Input::~Input() {
		m_Window = nullptr;
	}
	bool Input::IsKeyPressed(int keycode) {
		return glfwGetKey(m_Window, keycode) == GLFW_PRESS;
	}
	bool Input::IsMouseButtonPressed(int button) {
		return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
	}
	void Input::GetMousePosition(double& xpos, double& ypos) {
		glfwGetCursorPos(m_Window, &xpos, &ypos);
	}
	void Input::SetMousePosition(double xpos, double ypos) {
		glfwSetCursorPos(m_Window, xpos, ypos);
	}
	void Input::SetMouseVisible(bool visible) {
		glfwSetInputMode(m_Window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}