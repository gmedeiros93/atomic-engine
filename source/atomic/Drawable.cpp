#include "Drawable.h"

using namespace atomic;

Drawable::Drawable(GLenum type) : type(type), count(0)
{
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V
		0.0f, 0.8f, 0.0f, 0.5f, 1.0f,
		-0.8f, -0.8f, 0.0f, 0.0f, 0.0f,
		0.8f, -0.8f, 0.0f, 1.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	this->count = 3;
}

Drawable::~Drawable()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Drawable::setTexture(Texture *texture)
{
	this->texture = texture;
}

void Drawable::upload(const GLfloat *data, GLuint count)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

void Drawable::draw()
{
	GLint currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLint loc_vert = glGetAttribLocation(currentProgram, "vert");
	GLint loc_vertTexCoord = glGetAttribLocation(currentProgram, "vertTexCoord");

	glEnableVertexAttribArray(loc_vert);
	glEnableVertexAttribArray(loc_vertTexCoord);
	
	glVertexAttribPointer(loc_vert, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(loc_vertTexCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
	
	if (texture != NULL)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->object());
		glUniform1ui(glGetUniformLocation(currentProgram, "tex"), 0);
	}

	glDrawArrays(type, 0, count);
}