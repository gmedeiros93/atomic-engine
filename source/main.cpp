#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <sstream>

#include "atomic/Shader.h"
#include "atomic/Program.h"
#include "atomic/Bitmap.h"
#include "atomic/Texture.h"
#include "atomic/Camera.h"

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
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Upload vertex data
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V
		 0.0f,  0.8f, 0.0f, 0.5f, 1.0f,
		-0.8f, -0.8f, 0.0f, 0.0f, 0.0f,
		 0.8f, -0.8f, 0.0f, 1.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// [0, 1, 2] -> vert
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	// [3, 4] -> vertTexCoord
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

static GLuint load_box(atomic::Program *program)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Upload vertex data
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V  Normal
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-1.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f, 1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 1.0f, 1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-1.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,

		// front
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

		// left
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		// right
		1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// [0, 1, 2] -> vert
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), NULL);

	// [3, 4] -> vertTexCoord
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	// [5, 6, 7] -> vertNormal
	glEnableVertexAttribArray(program->attrib("vertNormal"));
	glVertexAttribPointer(program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(5 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

glm::vec3 handle_mouse(GLFWwindow *window)
{
	int width, height;
	double mouse_x, mouse_y;

	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &mouse_x, &mouse_y);

	double center_x = (float)width / 2.0f;
	double center_y = (float)height / 2.0f;

	double yaw = mouse_x - center_x;
	double pitch = mouse_y - center_y;

	glfwSetCursorPos(window, center_x, center_y);
	return glm::vec3(yaw, pitch, 0.0f);
}

void atomic_main(int argc, char *argv[])
{
	glfwSetErrorCallback(glfw_error);

	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Atomic", NULL, NULL);

	if (window == NULL)
		throw std::runtime_error("Failed to create window");

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
		throw std::runtime_error((
		std::string("Failed to initialize GLEW: ") + (char *)glewGetErrorString(err)
		).c_str());

	glClearColor(0.225f, 0.15f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);

	std::vector<atomic::Shader> shaders;
	shaders.push_back(atomic::Shader::fromFile("main_vert.glsl", GL_VERTEX_SHADER));
	shaders.push_back(atomic::Shader::fromFile("main_frag.glsl", GL_FRAGMENT_SHADER));
	atomic::Program *program = new atomic::Program(shaders);
	program->use();

	//atomic::Bitmap bitmap = atomic::Bitmap::fromFile("hazard.png");
	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("wooden-crate.jpg");
	bitmap.flipVertically();
	atomic::Texture *texture = new atomic::Texture(bitmap);

	atomic::Camera *camera = new atomic::Camera();
	camera->setPosition(glm::vec3(0,0,4));
	camera->setClippingPlanes(0.1f, 10.0f);

	//GLuint triangle = load_triangle(program);
	GLuint box = load_box(program);

	double delta = 0.0;
	double update_title_accum = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		int width, height;

		camera->look(handle_mouse(window) * 0.5f);

		glfwGetFramebufferSize(window, &width, &height);
		float aspect = (float)(width) / (float)(height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::rotate(glm::mat4(), fmodf(((float)(time)) * 45.0f, 360.0f), glm::vec3(0,1,0));

		program->setUniform("projection", camera->getProjectionMatrix(aspect));
		program->setUniform("view", camera->getViewMatrix());
		program->setUniform("model", model);

		program->setUniform("lightPos", glm::vec3(3,3,3));
		program->setUniform("cameraPos", camera->getPosition());

		program->setUniform("time", time);
		program->setUniform("sun", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
		//glUniform1f(program->uniform("time"), (GLfloat)time);
		//glBindVertexArray(triangle);
		glBindVertexArray(box);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->object());
		program->setUniform("tex", 0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6*2*3);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		delta = glfwGetTime() - time;
		
		// update
		const float moveSpeed = 10.0f;

		float k_w = glfwGetKey(window, GLFW_KEY_W) ?  1.0f : 0.0f;
		float k_s = glfwGetKey(window, GLFW_KEY_S) ? -1.0f : 0.0f;
		float k_d = glfwGetKey(window, GLFW_KEY_D) ?  1.0f : 0.0f;
		float k_a = glfwGetKey(window, GLFW_KEY_A) ? -1.0f : 0.0f;
		float k_z = glfwGetKey(window, GLFW_KEY_Z) ?  1.0f : 0.0f;
		float k_x = glfwGetKey(window, GLFW_KEY_X) ? -1.0f : 0.0f;

		float forward = k_w + k_s;
		float right = k_d + k_a;
		float up = k_z + k_x;

		if (forward != 0.0f)
			camera->move(camera->getForwardVector() * moveSpeed * forward * (float)delta);

		if (right != 0.0f)
			camera->move(camera->getRightVector() * moveSpeed * right * (float)delta);

		if (up != 0.0f)
			camera->move(glm::vec3(0,1,0) * moveSpeed * up * (float)delta);

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
		atomic_main(argc, argv);
	}
	catch (std::exception e)
	{
		fprintf(stderr, "Uncaught exception: %s\n", e.what());
		return 1;
	}

	return 0;
}