#include <iostream>
#include "Window.h"

namespace Engine {
	Window::Window(int width, int height, const char* title)
		: m_Width(width), m_Height(height), m_Title(title) {

		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW"); // dok ne skontam sta cu drugo ma da i ovo je ok
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 8);
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);

		if (!m_Window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window"); // isto tako
		}

		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD"); // i ovo
		}

		glEnable(GL_MULTISAMPLE);
	}

	Window::~Window() {
		if (m_Window) {
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}

		glfwTerminate();
	}

	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(m_Window);
	}

	void Window::CloseWindow() const {
		glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
	}

	void Window::PollEvents() const {
		glfwPollEvents();
	}

	void Window::SwapBuffers() const {
		glfwSwapBuffers(m_Window);
	}
}