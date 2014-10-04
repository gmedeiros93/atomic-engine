#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

using namespace atomic;

Shader::Shader(const std::string& shaderCode, GLenum shaderType) : _object(0), _refCount(NULL)
{
    _object = glCreateShader(shaderType);

    if (_object == 0)
        throw std::runtime_error("glCreateShader failed");

	const char *code = shaderCode.c_str();
	
    glShaderSource(_object, 1, (const GLchar**)&code, NULL);
    glCompileShader(_object);

    GLint status;
    glGetShaderiv(_object, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
	{
		printf("Shader compilation failed\n");
        
        GLint infoLogLength;
        glGetShaderiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(_object, infoLogLength, NULL, strInfoLog);

		printf("%s", strInfoLog);
        delete[] strInfoLog;
        
        glDeleteShader(_object);
		_object = 0;

        throw std::runtime_error("Shader compilation failed");
    }
    
    _refCount = new unsigned;
    *_refCount = 1;
}

Shader::~Shader()
{
	if (_refCount)
		_release();
}

Shader Shader::fromFile(const char *filename, GLenum shaderType)
{
	std::ifstream f;
	f.open(filename, std::ios::in | std::ios::binary);

	if (!f.is_open())
		throw std::runtime_error(std::string("Failed to open shader file: ") + filename);

	//read whole file into stringstream buffer
	std::stringstream buffer;
	buffer << f.rdbuf();

	Shader shader(buffer.str(), shaderType);
	return shader;
}

GLuint Shader::object() const
{
    return _object;
}

Shader::Shader(const Shader& other) : _object(other._object), _refCount(other._refCount)
{
	_retain();
}

Shader& Shader::operator =(const Shader& other)
{
    _release();
    _object = other._object;
    _refCount = other._refCount;
    _retain();
    return *this;
}

void Shader::_retain()
{
    assert(_refCount);
    *_refCount += 1;
}

void Shader::_release()
{
    assert(_refCount && *_refCount > 0);
    *_refCount -= 1;

    if (*_refCount == 0)
	{
        glDeleteShader(_object);
		_object = 0;

        delete _refCount;
		_refCount = NULL;
    }
}

