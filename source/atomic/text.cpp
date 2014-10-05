#include "text.h"

using namespace atomic;

FT_Library ft = nullptr;
atomic::Program *textProgram = nullptr;
GLuint tex = 0;
GLuint vbo = 0;

FontFace::FontFace(const char *filename, long face_index)
{
	if (ft == nullptr)
	{
		if (FT_Init_FreeType(&ft))
		{
			throw_err(atomic::error, "FT_Init_FreeType failed");
			return;
		}
	}

	if (textProgram == nullptr)
		textProgram = Program::fromCommonFiles("shaders/text_vert.glsl", "shaders/text_frag.glsl");

	if (tex == 0)
	{
		glActiveTexture(GL_TEXTURE0);
		
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		textProgram->use();
		textProgram->setUniform("tex", 0);
	}

	if (vbo == 0)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}

	if (FT_New_Face(ft, filename, 0, &face))
	{
		throw_err(atomic::error, "Could not open font");
		return;
	}
}

FontFace::~FontFace()
{
}

void FontFace::setSize(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	FT_Set_Pixel_Sizes(face, width, height);
}

void FontFace::render(const char *text, float x, float y, float sx, float sy, glm::mat4 matrix)
{
	textProgram->use();
	textProgram->setUniform("color", glm::vec4(0, 0, 0, 1));;
	textProgram->setUniform("matrix", matrix);
	textProgram->assignTexture("tex", 0, tex);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(textProgram->attrib("coord"));
	glVertexAttribPointer(textProgram->attrib("coord"), 4, GL_FLOAT, GL_FALSE, 0, 0);
	glDisable(GL_DEPTH_TEST);

	//glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//return;

	const char *p;

	for (p = text; *p; p++)
	{
		if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
			continue;

		FT_GlyphSlot g = face->glyph;

		//printf("trying to render %c\n", *p);
		//printf("%d %d\n", g->bitmap.width, g->bitmap.rows);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_ALPHA,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_ALPHA,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		/*GLfloat box[4][4] = {
				{ x2, -y2, 0, 0 },
				{ x2 + w, -y2, 1, 0 },
				{ x2, -y2 - h, 0, 1 },
				{ x2 + w, -y2 - h, 1, 1 },
		};*/

		GLfloat box[] = {
			x, y, 0, 1,
			x + g->bitmap.width * sx, y, 1, 1,
			x, y + g->bitmap.rows * sy, 0, 0,
			x + g->bitmap.width * sx, y + g->bitmap.rows * sy, 1, 0
		};

		/*GLfloat box[] = {
			-1.0, -1.0, 0, 0,
			 1.0, -1.0, 1, 0,
			-1.0,  1.0, 0, 1,
			 1.0,  1.0, 1, 1,
		};*/

		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glEnable(GL_DEPTH_TEST);
}