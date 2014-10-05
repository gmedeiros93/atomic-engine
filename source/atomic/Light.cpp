#include "Light.h"

using namespace atomic;

const char *Light::getUniformName(int index, const char *attribute)
{
	std::stringstream name;
	name << "lights[";
	name << index;
	name << "].";
	name << attribute;
	return strdup(name.str().c_str());
}

void Light::upload(Program *program, int index)
{
	program->setUniform(getUniformName(index, "type"), getType());
	program->setUniform(getUniformName(index, "diffuse"), this->diffuse);
	program->setUniform(getUniformName(index, "specular"), this->specular);
}

int DirLight::getType()
{
	return DIR_LIGHT;
}

void DirLight::upload(Program *program, int index)
{
	program->setUniform(getUniformName(index, "dir"), this->dir);
	program->setUniform(getUniformName(index, "ambient"), this->ambient);
	Light::upload(program, index);
}

int PointLight::getType()
{
	return POINT_LIGHT;
}

void PointLight::upload(Program *program, int index)
{
	program->setUniform(getUniformName(index, "pos"), this->pos);
	Light::upload(program, index);
	program->setUniform(getUniformName(index, "falloff1"), this->linear);
	program->setUniform(getUniformName(index, "falloff2"), this->quadratic);
}

int SpotLight::getType()
{
	return SPOT_LIGHT;
}

void SpotLight::upload(Program *program, int index)
{
	program->setUniform(getUniformName(index, "pos"), this->pos);
	program->setUniform(getUniformName(index, "dir"), this->dir);
	Light::upload(program, index);
	program->setUniform(getUniformName(index, "falloff1"), this->inner);
	program->setUniform(getUniformName(index, "falloff2"), this->outer);
}