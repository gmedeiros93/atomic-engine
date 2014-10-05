#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "exceptions.h"
#include "atomic/ClientReactor.h"
#include "atomic/Model.h"

void atomic_main(int argc, char *argv[])
{
	atomic::ClientReactor *reactor = new atomic::ClientReactor("Atomic", 1280, 720);

	atomic::ModelAsset *asset = atomic::from_bin("models/lost_empire.bin");
	atomic::ModelInstance *instance = new atomic::ModelInstance(asset);

	reactor->camera->setPosition(glm::vec3(0, 0, 4));
	instance->setPosition(glm::vec3(0.0f, -50.0f, 0.0f));
	//instance->setPosition(glm::vec3(0.0f, -5.0f, 0.0f));

	reactor->addInstance(instance);
	reactor->run();

	delete reactor;
}

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	try
	{
#endif
		atomic_main(argc, argv);
#ifndef _DEBUG
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
#endif

	return 0;
}