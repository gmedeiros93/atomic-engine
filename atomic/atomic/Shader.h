#ifndef __SHADER_H
#define __SHADER_H

#include "GL/glew.h"
#include <vector>

namespace atomic
{
	class Shader {
	public:
		static Shader fromFile(const std::string& filePath, GLenum shaderType);

		Shader(const std::string& shaderCode, GLenum shaderType);
		Shader(const Shader& other);
		~Shader();

		Shader& operator =(const Shader& other);

		GLuint object() const;

	private:
		GLuint _object;
		unsigned* _refCount;
		void _retain();
		void _release();
	};
}

#endif