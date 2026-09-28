#pragma once

#include <iostream>
#include <glad/glad.h>	
#include <GLFW/glfw3.h>

namespace Engine {
	class Window {
	public:
		Window(int width, int height, const char* title);
		virtual ~Window();
		bool ShouldClose() const;
		void CloseWindow() const;
		void PollEvents() const;
		void SwapBuffers() const;

		GLFWwindow* GetNativeWindow() const { return m_Window; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
	private:
		int m_Width;
		int m_Height;
		const char* m_Title;
		GLFWwindow* m_Window;
	};
}

