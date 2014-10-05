#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../exceptions.h"
#include "Program.h"

extern FT_Library ft;
extern atomic::Program *textProgram;
extern GLuint tex;
extern GLuint vbo;

namespace atomic
{
	class FontFace
	{
	private:
		FT_Face face;

		unsigned int width = 0;
		unsigned int height = 0;

	public:
		FontFace(const char *filename, long face_index);
		~FontFace();

		void setSize(unsigned int width, unsigned int height);
		FT_GlyphSlot loadChar(char character, FT_Int32 flags = FT_LOAD_RENDER);

		void render(const char *text, float x, float y, float sx, float sy, glm::mat4 matrix);
	};
}

FT_Face loadFace(const char *filename, FT_Long face_index);