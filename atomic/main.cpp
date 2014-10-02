#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <cassert>

#include "atomic/Shader.h"
#include "atomic/Program.h"
#include "atomic/Bitmap.h"
#include "atomic/Texture.h"

#ifdef _WIN32
	#undef APIENTRY
	#include "windows.h"
	#define sleep Sleep
#else
	//other platform sleep
	//#define sleep usleep
#endif

void glfw_error(int code, const char *msg)
{
	std::string what = "GLFW error ";
	what += code;
	what += ": ";
	what += msg;
    throw std::runtime_error(what);
}

static GLuint load_triangle(atomic::Program *program)
{
	// make and bind the VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// make and bind the VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V
		 0.0f, 0.8f, 0.0f,   0.5f, 1.0f,
		-0.8f,-0.8f, 0.0f,   0.0f, 0.0f,
		 0.8f,-0.8f, 0.0f,   1.0f, 0.0f,
	};
	//GLfloat vertexData[] = {
	//	//  X     Y     Z       U     V
	//	 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	//	 1.0f,-1.0f, 0.0f,  -1.0f, 1.0f,
	//	-1.0f,-1.0f, 0.0f,  -1.0f, 1.0f,

	//	-1.0f,-1.0f, 0.0f,  -1.0f,-1.0f,
	//	-1.0f, 1.0f, 0.0f,   1.0f,-1.0f,
	//	 1.0f, 1.0f, 0.0f,   1.0f,-1.0f,
	//};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	// connect the uv to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

void _main()
{
	glfwSetErrorCallback(glfw_error);

	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_DECORATED, false);

	//GLFWwindow *window = glfwCreateWindow(1920, 1080, "Atomic", NULL, NULL);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Atomic", NULL, NULL);

	if (window == NULL)
		throw std::runtime_error("Failed to create window");

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();

	if (err != GLEW_OK)
		throw std::runtime_error((
			std::string("Failed to initialize GLEW: ") + (char *)glewGetErrorString(err)
		).c_str());

	glClearColor(0.225f, 0.2f, 0.2f, 0.0f);

	double delta = 0.0;
	//double target_delta = 1.0 / 60;
	double target_delta = 0.0;
	double update_title_accum = 0.0;

	std::vector<atomic::Shader> shaders;
	shaders.push_back(atomic::Shader::fromFile("main_vert.glsl", GL_VERTEX_SHADER));
	shaders.push_back(atomic::Shader::fromFile("main_frag.glsl", GL_FRAGMENT_SHADER));
	atomic::Program *program = new atomic::Program(shaders);
	program->activate();

	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("hazard.png");
	bitmap.flipVertically();
	atomic::Texture *texture = new atomic::Texture(bitmap);

	GLuint triangle = load_triangle(program);

	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform1f(program->uniform("time"), (GLfloat)time);
		glBindVertexArray(triangle);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->object());
		program->setUniform("tex", 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		delta = glfwGetTime() - time;

		if (delta < target_delta)
		{
			sleep((unsigned long)((target_delta - delta) * 1000.0f));
			delta = glfwGetTime() - time;
		}

		// show FPS in title
		update_title_accum += delta;

		if (update_title_accum >= 0.2f)
		{
			update_title_accum -= 0.2f;
			std::stringstream title;
			title << "Atomic - ";
			title << (int)floor(1.0f / delta + 0.5);
			glfwSetWindowTitle(window, title.str().c_str());
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, 1);
	}

	glfwTerminate();
}

int main(int argc, char *argv[])
{
	try
	{
		_main();
	}
	catch (const std::exception &e)
	{
		fprintf(stderr, "std::exception: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}