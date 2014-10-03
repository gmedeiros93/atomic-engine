#include "ClientReactor.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace atomic;

ClientReactor::ClientReactor(Window *window) : window(window)
{
	camera = new Camera();
}

ClientReactor::~ClientReactor()
{
	delete camera;
}