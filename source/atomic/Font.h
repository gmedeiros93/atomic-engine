#pragma once

#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../exceptions.h"
#include "Program.h"

extern atomic::Program *textProgram;
extern GLuint tex;
extern GLuint vbo;

namespace atomic
{
	struct Glyph
	{
		struct { float x, y; } advance;
		struct { float x, y; } dimensions;
		struct { float x, y; } offset;
		float bitmap_left;
		float bitmap_top;
		unsigned char *buffer;
	};

	class Font
	{
	private:
		FT_Library library;
		FT_Face face;

		GLuint texture;
		int texture_size[2];
		
		//Glyph glyphs[256];
		std::unordered_map<char, Glyph> glyphs;
		int height;

	public:
		Font(const char *filename, int size);
		~Font();
	};

	Font::Font(const char *filename, int size)
	{
		FT_Init_FreeType(&library);
		FT_New_Face(library, filename, 0, &face);
		FT_Set_Pixel_Sizes(face, 0, size);

		FT_GlyphSlot g = face->glyph;
		int max_width = 1024;
		int row_width = 0;
		int row_height = 0;

		const char *chars = " !\"#$%&'`()*+,-_./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^abcdefghijklmnopqrstuvwxyz{|}~³";

		for (const char *p = chars; *p; p++)
		{
			if (!FT_Load_Char(face, *p, FT_LOAD_RENDER))
				continue;

			if (row_width + g->bitmap.width + 1 >= max_width)
			{
				texture_size[0] = row_width > texture_size[0] ? row_width : texture_size[0];
				texture_size[1] += row_height;
				row_width = 0;
				row_height = 0;
			}

			Glyph glyph;

			glyph.advance.x = g->advance.x >> 6;
			glyph.advance.y = g->advance.y >> 6;
			glyph.dimensions.x = g->bitmap.width;
			glyph.dimensions.y = g->bitmap.rows;
			glyph.offset.x = g->bitmap_left;
			glyph.offset.y = g->bitmap_top;
			glyph.buffer = g->bitmap.buffer;

			row_width += (int)glyph.dimensions.x + 1;
			row_height = row_height > g->bitmap.rows ? row_height : g->bitmap.rows;
			height = row_height > height ? row_height : height;

			//glyphs[*p] = glyph;
			glyphs.insert(*p, glyph);
		}

		glActiveTexture(0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_size[0], texture_size[1], 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int offset[] = {0, 0};
		row_height = 0;

		for (const char *p = chars; *p; p++)
		{
			Glyph glyph = glyphs[*p];

			if (glyph)
				continue;
	}
}

FT_Face loadFace(const char *filename, FT_Long face_index);