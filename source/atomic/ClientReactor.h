#pragma once

#include <list>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Camera.h"
#include "Model.h"
#include "Window.h"

namespace atomic
{
	class ClientReactor
	{
		Window *window;
		Camera *camera;
		std::list<ModelInstance*> instances;

	public:
		ClientReactor(Window *window);
		~ClientReactor();
	};
};