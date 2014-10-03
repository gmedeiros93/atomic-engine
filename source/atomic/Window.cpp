#include <cassert>
#include <cstdlib>

#include "Window.h"

using namespace atomic;

bool _glfwInitialized = false;

Window::Window(char *title)
{
	_init();

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode;
	
	if (monitor != NULL)
		mode = glfwGetVideoMode(monitor);

	if (mode == NULL)
		throw std::runtime_error("Cannot determine suitable fullscreen mode");

	glfwWindowHint(GLFW_DECORATED, 0);
	_create(title, mode->width, mode->height);
}

Window::Window(char *title, int width, int height)
{
	_init();
	_create(title, width, height);
}

Window::~Window()
{
	glfwTerminate();
}

void Window::_init()
{
	if (!_glfwInitialized)
	{
		if (!glfwInit())
			throw std::runtime_error("glfwInit failed");

		atexit(glfwTerminate);
		_glfwInitialized = true;
	}
	else
		glfwDefaultWindowHints();

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_SAMPLES, 4);
}

void Window::_create(char *title, int width, int height)
{
	handle = glfwCreateWindow(width, height, title, NULL, NULL);

	if (handle == NULL)
		throw std::runtime_error("Cannot create window");

	GLFWwindow *current = glfwGetCurrentContext();

	glfwMakeContextCurrent(handle);
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
		throw std::runtime_error((
			std::string("glewInit failed: ") +
			(char *)glewGetErrorString(err)
		).c_str());

	glClearColor(0.225f, 0.15f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	if (current != NULL)
		glfwMakeContextCurrent(current);
}