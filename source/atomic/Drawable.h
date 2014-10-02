#pragma once

#include "Texture.h"

namespace atomic
{
	class Drawable
	{
	public:
		Drawable(GLenum type);
		~Drawable();

		void setTexture(Texture *texture);
		void upload(const GLfloat *data, GLuint count);
		void draw();

		GLuint vao;
	private:
		Texture *texture;

		GLenum type;
		GLuint count;

		GLuint vbo;
	};
}