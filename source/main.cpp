#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <sstream>
#include <list>

#include "atomic/Shader.h"
#include "atomic/Program.h"
#include "atomic/Bitmap.h"
#include "atomic/Texture.h"
#include "atomic/Camera.h"
#include "atomic/Model.h"
#include "atomic/Window.h"

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

GLFWwindow *atomic_init()
{
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	return window;
}

GLuint create_gizmo()
{
	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLfloat data[] = {
		// X+
		0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,

		// Y+
		0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,

		// Z+
		0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	return vbo;
}

void atomic_main(int argc, char *argv[])
{
	//GLFWwindow *window = atomic_init();
	atomic::Window *trueWindow = new atomic::Window("Atomic", 1280, 720);
	GLFWwindow *window = trueWindow->getHandle();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::list<atomic::ModelInstance*> instances;
	atomic::Camera *camera = new atomic::Camera();
	camera->setPosition(glm::vec3(0, 0, 4));

	std::vector<atomic::Shader> shaders;
	shaders.push_back(atomic::Shader::fromFile("main_vert.glsl", GL_VERTEX_SHADER));
	shaders.push_back(atomic::Shader::fromFile("main_frag.glsl", GL_FRAGMENT_SHADER));
	atomic::Program *program = new atomic::Program(shaders);

	shaders.clear();
	//std::vector<atomic::Shader> shaders;
	shaders.push_back(atomic::Shader::fromFile("prim_vert.glsl", GL_VERTEX_SHADER));
	shaders.push_back(atomic::Shader::fromFile("prim_frag.glsl", GL_FRAGMENT_SHADER));
	atomic::Program *program_prim = new atomic::Program(shaders);
	
	atomic::ModelAsset *boxAsset = create_box_asset(program);
	atomic::ModelAsset *teapotAsset = from_bin(program, "lost_empire_rand.bin");
	
	atomic::ModelInstance *teapot = new atomic::ModelInstance(teapotAsset);
	//teapot->setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
	teapot->setPosition(glm::vec3(0.0f, -50.0f, 0.0f));
	instances.push_back(teapot);//

	/*for (int i = 0; i < 50; i++)
	{
		atomic::ModelInstance *box = new atomic::ModelInstance(teapot);
		box->setPosition(glm::vec3(sin(i)*8, cos(i)*8, -sin(i)*8));
		instances.push_back(box);
	}*/

	double delta = 0.0;
	double update_title_accum = 0.0;

	int had_focus = false;
	GLuint gizmo = create_gizmo();

	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		int width, height;

		glm::vec3 mouse_input = handle_mouse(window);

		if (had_focus)
			camera->look(mouse_input * 0.5f);

		had_focus = glfwGetWindowAttrib(window, GLFW_FOCUSED);

		glfwGetFramebufferSize(window, &width, &height);
		float aspect = (float)(width) / (float)(height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::list<atomic::ModelInstance*>::iterator it;

		for (it = instances.begin(); it != instances.end(); ++it)
			(*it)->draw(camera, aspect);

		program_prim->use();
		program_prim->setUniform("view", camera->getViewMatrix());
		program_prim->setUniform("projection", camera->getProjectionMatrix(aspect));

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, gizmo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

		glDrawArrays(GL_LINES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnable(GL_DEPTH_TEST);

		program_prim->stopUsing();

		glfwSwapBuffers(window);
		glfwPollEvents();

		delta = glfwGetTime() - time;
		
		// update
		//for (it = instances.begin(); it != instances.end(); ++it)
		//	(*it)->setRotation(glm::rotate((*it)->getRotation(), (float)delta * 45.0f, glm::vec3(1, 0, 0)));

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
			title << delta;
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