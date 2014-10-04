#include "Light.h"

const char *atomic::lightUniformName(int index, const char *attribute)
{
	std::stringstream name;
	name << "lights[";
	name << index;
	name << "].";
	name << attribute;
	return strdup(name.str().c_str());
}