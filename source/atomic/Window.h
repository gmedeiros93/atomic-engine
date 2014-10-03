#pragma once

#include <list>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace atomic
{
	class Window
	{
		GLFWwindow *handle;

		void _init();
		void _create(char *title, int width, int height);
		
	public:
		Window(char *title);
		Window(char *title, int width, int height);
		~Window();

		GLFWwindow *getHandle() const { return handle; }
	};
};