#pragma once

#define MAX_LIGHTS 32

#define DIR_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#include <sstream>
#include <glm/glm.hpp>
#include "Program.h"

namespace atomic
{
	class Light
	{
	private:
		virtual int getType() = 0;

	protected:
		const char *getUniformName(int index, const char *attribute);

	public:
		glm::vec3 diffuse, specular;
		virtual void upload(Program *program, int index);
	};

	class DirLight : public Light
	{
	private:
		int getType();

	public:
		glm::vec3 dir;
		glm::vec3 ambient;

		virtual void upload(Program *progam, int index);
	};

	class PointLight : public Light
	{
	private:
		int getType();

	public:
		glm::vec3 pos;
		float linear, quadratic;

		virtual void upload(Program *progam, int index);
	};

	class SpotLight : public Light
	{
	private:
		int getType();

	public:
		glm::vec3 pos, dir;
		
		float inner;
		float outer;

		virtual void upload(Program *progam, int index);
	};
};