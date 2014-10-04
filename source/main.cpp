#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <sstream>
#include <list>

#include "exceptions.h"
#include "atomic/Shader.h"
#include "atomic/Program.h"
#include "atomic/Bitmap.h"
#include "atomic/Texture.h"
#include "atomic/Camera.h"
#include "atomic/Model.h"
#include "atomic/Window.h"
#include "atomic/ClientReactor.h"

void atomic_main(int argc, char *argv[])
{
	atomic::ClientReactor *reactor = new atomic::ClientReactor("Atomic", 1280, 720);
	atomic::ModelAsset *asset = from_bin(reactor->mainShader, "models/lost_empire.bin");

	atomic::ModelInstance *instance = new atomic::ModelInstance(asset);

	reactor->camera->setPosition(glm::vec3(0, 0, 4));
	instance->setPosition(glm::vec3(0.0f, -50.0f, 0.0f));

	reactor->addInstance(new atomic::ModelInstance(instance));
	reactor->run();
}

int main(int argc, char *argv[])
{
	try
	{
		atomic_main(argc, argv);
	}
	catch (atomic::error e)
	{
		fprintf(stderr, "Uncaught %s at %s: %s\n", e.type(), e.loc().c_str(), e.what());
		return 1;
	}
	catch (std::exception e)
	{
		fprintf(stderr, "Uncaught exception: %s\n", e.what());
		return 1;
	}

	return 0;
}