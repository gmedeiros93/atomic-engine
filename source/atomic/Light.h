#pragma once

#define MAX_LIGHTS 32

#define DIR_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#include <sstream>
#include <glm/glm.hpp>

namespace atomic
{
	struct Light
	{
		int type;

		glm::vec3 vec;
		glm::vec3 ambient, diffuse, specular;

		float linear, quadratic;
	};

	const char *lightUniformName(int index, const char *attribute);
};