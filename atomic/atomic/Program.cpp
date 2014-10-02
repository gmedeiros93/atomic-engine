#include <cassert>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include "Program.h"

using namespace atomic;

Program::Program(const std::vector<Shader>& shaders) :
	_object(0)
{
	if (shaders.empty())
		throw std::runtime_error("Cannot create program with no shaders");

	_object = glCreateProgram();
	if (_object == 0)
		throw std::runtime_error("glCreateProgram failed");

	for (unsigned i = 0; i < shaders.size(); ++i)
		glAttachShader(_object, shaders[i].object());

	glLinkProgram(_object);

	for (unsigned i = 0; i < shaders.size(); ++i)
		glDetachShader(_object, shaders[i].object());

	GLint status;
	glGetProgramiv(_object, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		std::string msg("Program linking failure: ");

		GLint infoLogLength;
		glGetProgramiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* strInfoLog = new char[infoLogLength + 1];
		glGetProgramInfoLog(_object, infoLogLength, NULL, strInfoLog);
		msg += strInfoLog;
		delete[] strInfoLog;

		glDeleteProgram(_object);
		_object = 0;

		throw std::runtime_error(msg);
	}
}

Program::~Program()
{
	if (_object != 0)
		glDeleteProgram(_object);
}

GLuint Program::object() const
{
	return _object;
}

void Program::activate() const
{
	glUseProgram(_object);
}

bool Program::isActive() const
{
	GLint currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	return (currentProgram == (GLint)_object);
}

GLint Program::attrib(const GLchar *name) const
{
	return glGetAttribLocation(_object, name);
}

GLint Program::uniform(const GLchar *name) const
{
	return glGetUniformLocation(_object, name);
}

#define ATTRIB_N_UNIFORM_SETTERS(OGL_TYPE, TYPE_PREFIX, TYPE_SUFFIX) \
\
	void Program::setAttrib(const GLchar* name, OGL_TYPE v0) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX (attrib(name), v0); } \
	void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX (attrib(name), v0, v1); } \
	void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX (attrib(name), v0, v1, v2); } \
	void Program::setAttrib(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX (attrib(name), v0, v1, v2, v3); } \
\
	void Program::setAttrib1v(const GLchar* name, const OGL_TYPE* v) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX ## v (attrib(name), v); } \
	void Program::setAttrib2v(const GLchar* name, const OGL_TYPE* v) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX ## v (attrib(name), v); } \
	void Program::setAttrib3v(const GLchar* name, const OGL_TYPE* v) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX ## v (attrib(name), v); } \
	void Program::setAttrib4v(const GLchar* name, const OGL_TYPE* v) \
		{ assert(isActive()); glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX ## v (attrib(name), v); } \
\
	void Program::setUniform(const GLchar* name, OGL_TYPE v0) \
		{ assert(isActive()); glUniform1 ## TYPE_SUFFIX (uniform(name), v0); } \
	void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1) \
		{ assert(isActive()); glUniform2 ## TYPE_SUFFIX (uniform(name), v0, v1); } \
	void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2) \
		{ assert(isActive()); glUniform3 ## TYPE_SUFFIX (uniform(name), v0, v1, v2); } \
	void Program::setUniform(const GLchar* name, OGL_TYPE v0, OGL_TYPE v1, OGL_TYPE v2, OGL_TYPE v3) \
		{ assert(isActive()); glUniform4 ## TYPE_SUFFIX (uniform(name), v0, v1, v2, v3); } \
\
	void Program::setUniform1v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
		{ assert(isActive()); glUniform1 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
	void Program::setUniform2v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
		{ assert(isActive()); glUniform2 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
	void Program::setUniform3v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
		{ assert(isActive()); glUniform3 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
	void Program::setUniform4v(const GLchar* name, const OGL_TYPE* v, GLsizei count) \
		{ assert(isActive()); glUniform4 ## TYPE_SUFFIX ## v (uniform(name), count, v); }

ATTRIB_N_UNIFORM_SETTERS(GLfloat, , f);
ATTRIB_N_UNIFORM_SETTERS(GLdouble, , d);
ATTRIB_N_UNIFORM_SETTERS(GLint, I, i);
ATTRIB_N_UNIFORM_SETTERS(GLuint, I, ui);

void Program::setUniformMatrix2(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix2fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix3(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix3fv(uniform(name), count, transpose, v);
}

void Program::setUniformMatrix4(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix4fv(uniform(name), count, transpose, v);
}

void Program::setUniform(const GLchar* name, const glm::mat2& m, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix2fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat3& m, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix3fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* name, const glm::mat4& m, GLboolean transpose)
{
	assert(isActive());
	glUniformMatrix4fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec3& v)
{
	setUniform3v(uniformName, glm::value_ptr(v));
}

void Program::setUniform(const GLchar* uniformName, const glm::vec4& v)
{
	setUniform4v(uniformName, glm::value_ptr(v));
}