#pragma once

#include <GL/glew.h>
#include <string>

namespace atomic
{
    class Shader
	{
    public:
        Shader(const std::string& shaderCode, GLenum shaderType);
		~Shader();

		static Shader fromFile(const char *filename, GLenum shaderType);

        GLuint object() const;

        Shader(const Shader& other);
        Shader& operator =(const Shader& other);

    private:
        GLuint _object;
        unsigned* _refCount;
        
        void _retain();
        void _release();
    };
    
}
