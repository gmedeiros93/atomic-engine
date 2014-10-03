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

	std::list<atomic::ModelInstance*> instances;
	atomic::Camera *camera = new atomic::Camera();
	camera->setPosition(glm::vec3(0, 0, 4));

	std::vector<atomic::Shader> shaders;
	shaders.push_back(atomic::Shader::fromFile("main_vert.glsl", GL_VERTEX_SHADER));
	shaders.push_back(atomic::Shader::fromFile("main_frag.glsl", GL_FRAGMENT_SHADER));
	atomic::Program *program = new atomic::Program(shaders);
	
	atomic::ModelAsset *boxAsset = create_box_asset(program);
	//instances.push_back(new atomic::ModelInstance(boxAsset));

	//atomic::ModelAsset *teapot = from_obj(program, "test.obj");
	double time = glfwGetTime();
	atomic::ModelAsset *teapot = from_bin(program, "out.bin");
	double spent = glfwGetTime() - time;
	printf("Loaded %d vertices in %f seconds", teapot->drawCount, spent);

	atomic::ModelInstance *t = new atomic::ModelInstance(teapot);
	t->setTransform(glm::translate(glm::mat4(), glm::vec3(-1.0f, -2.0f, -5.0f)));
	instances.push_back(t);

	/*for (int i = 0; i < 50; i++)
	{
		atomic::ModelInstance *box = new atomic::ModelInstance(teapot);
		box->setTransform(glm::translate(glm::mat4(), glm::vec3(sin(i)*8, cos(i)*8, -sin(i)*8)));
		instances.push_back(box);
	}*/

	double delta = 0.0;
	double update_title_accum = 0.0;

	int had_focus = false;

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
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		delta = glfwGetTime() - time;
		
		// update
		for (it = instances.begin(); it != instances.end(); ++it)
			(*it)->setTransform(glm::rotate((*it)->getTransform(), (float)delta * 45.0f, glm::vec3(1, 0, 0)));

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