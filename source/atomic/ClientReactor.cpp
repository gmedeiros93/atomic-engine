#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../exceptions.h"
#include "ClientReactor.h"
#include "Camera.h"
#include "Shader.h"

using namespace atomic;

bool _glfwInitialized = false;

void _glfw_framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	ClientReactor *reactor = (ClientReactor *)glfwGetWindowUserPointer(window);
	reactor->framebufferSizeChanged(width, height);
}

ClientReactor::ClientReactor(const char *title, int width, int height)
{
	if (!_glfwInitialized)
	{
		if (!glfwInit())
			throw std::runtime_error("glfwInit failed");

		atexit(glfwTerminate);
		_glfwInitialized = true;
	}
	else
		glfwDefaultWindowHints();

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (window == NULL)
		throw std::runtime_error("Cannot create window");

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, _glfw_framebufferSizeCallback);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
		throw std::runtime_error((
			std::string("glewInit failed: ") + (char *)glewGetErrorString(err)
		).c_str());

	glClearColor(0.225f, 0.15f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	camera = new Camera();
	aspectRatio = (float)width / (float)height;

	primitiveShader = Program::fromCommonFiles("shaders/primitive_vert.glsl", "shaders/primitive_frag.glsl");
	postShader = Program::fromCommonFiles("shaders/post_vert.glsl", "shaders/post_frag.glsl");

	// Create backbuffer for post-processing
	// Texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Depth buffer
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// Framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw_err(atomic::error, "GL_FRAMEBUFFER is not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// VBO used for mapping postproc texture to screen
	GLfloat fbo_vertices[] = {
		-1.0, -1.0,
		 1.0, -1.0,
		-1.0,  1.0,
		 1.0,  1.0,
	};

	glGenBuffers(1, &vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Postproc shader
	attribute_v_coord_postproc = glGetAttribLocation(postShader->object(), "v_coord");
	uniform_fbo_texture = glGetAttribLocation(postShader->object(), "fbo_texture");
}

ClientReactor::~ClientReactor()
{
	glDeleteBuffers(1, &vbo_fbo_vertices);
	glDeleteRenderbuffers(1, &rbo_depth);
	glDeleteTextures(1, &fbo_texture);
	glDeleteFramebuffers(1, &fbo);

	delete postShader;
	delete primitiveShader;

	delete camera;

	glfwDestroyWindow(window);
}

void ClientReactor::addInstance(ModelInstance *instance)
{
	instances.push_back(instance);
}

void ClientReactor::removeInstance(ModelInstance *instance)
{
	std::vector<ModelInstance*>::iterator it;

	for (it = instances.begin(); it != instances.end(); it++)
	{
		if (*it == instance)
		{
			instances.erase(it);
			return;
		}
	}

	fprintf(stderr, "[warn] ClientReactor::removeInstance - instance not part of ModelInstance vector");
}

void ClientReactor::run()
{
	glfwSetWindowShouldClose(window, 0);

	bool hadFocus = false;
	double delta = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		
		draw();
		update(delta, hadFocus);
		finishDraw();

		glfwPollEvents();

		hadFocus = glfwGetWindowAttrib(window, GLFW_FOCUSED) == 1;
		delta = glfwGetTime() - time;
	}

	glfwSetWindowShouldClose(window, 0);
}

void ClientReactor::stop()
{
	glfwSetWindowShouldClose(window, 1);
}

void ClientReactor::framebufferSizeChanged(int width, int height)
{
	aspectRatio = (float)width / (float)height;

	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glViewport(0, 0, width, height);
}

void ClientReactor::draw()
{
	glfwMakeContextCurrent(window);

	// Draw world into fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPointSize(8.0);
	//glLineWidth(4.0);
	//glLineStipple(4, 2);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::vector<ModelInstance*>::iterator it;

	for (it = instances.begin(); it != instances.end(); ++it)
		(*it)->draw(camera, aspectRatio, (float)glfwGetTime());

	drawGizmo();
	//return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw fbo to screen with postproc
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(postShader->object());
	postShader->use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, fbo_texture);
	//postShader->setUniform("fbo_texture", 0);
	//glUniform1i(uniform_fbo_texture, 0);

	postShader->assignTexture("fbo_texture", 0, fbo_texture);
	postShader->assignTexture("rbo_depth", 1, rbo_depth, GL_RENDERBUFFER);
	
	postShader->setUniform("time", glfwGetTime());
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);

	glEnableVertexAttribArray(attribute_v_coord_postproc);
	glVertexAttribPointer(attribute_v_coord_postproc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(attribute_v_coord_postproc);
	//
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	// Extra
	//drawGizmo();
}

void ClientReactor::finishDraw()
{
	glfwSwapBuffers(window);
}

void ClientReactor::update(double delta, bool hadFocus)
{
	const float moveSpeed = 10.0f;
	const float lookSpeed = 0.5f;

	int width, height;
	double mouse_x, mouse_y;

	glfwGetWindowSize(window, &width, &height);
	glfwGetCursorPos(window, &mouse_x, &mouse_y);

	double center_x = (float)width / 2.0f;
	double center_y = (float)height / 2.0f;

	double yaw = mouse_x - center_x;
	double pitch = mouse_y - center_y;

	glfwSetCursorPos(window, center_x, center_y);

	if (hadFocus)
		camera->look(glm::vec3(glm::radians(yaw), glm::radians(pitch), 0.0f) * lookSpeed);
	
	float k_w = glfwGetKey(window, GLFW_KEY_W) ? 1.0f : 0.0f;
	float k_s = glfwGetKey(window, GLFW_KEY_S) ? -1.0f : 0.0f;
	float k_d = glfwGetKey(window, GLFW_KEY_D) ? 1.0f : 0.0f;
	float k_a = glfwGetKey(window, GLFW_KEY_A) ? -1.0f : 0.0f;
	float k_x = glfwGetKey(window, GLFW_KEY_X) ? 1.0f : 0.0f;
	float k_z = glfwGetKey(window, GLFW_KEY_Z) ? -1.0f : 0.0f;

	float forward = k_w + k_s;
	float right = k_d + k_a;
	float up = k_x + k_z;

	if (forward != 0.0f)
		camera->move(camera->getForwardVector() * moveSpeed * forward * (float)delta);

	if (right != 0.0f)
		camera->move(camera->getRightVector() * moveSpeed * right * (float)delta);

	if (up != 0.0f)
		camera->move(glm::vec3(0, 1, 0) * moveSpeed * up * (float)delta);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, 1);
}

void ClientReactor::drawGizmo()
{
	if (gizmo == 0)
	{
		glGenBuffers(1, &gizmo);
		glBindBuffer(GL_ARRAY_BUFFER, gizmo);

		GLfloat data[] = {
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	}
	else
		glBindBuffer(GL_ARRAY_BUFFER, gizmo);

	glDisable(GL_DEPTH_TEST);
	glUseProgram(primitiveShader->object());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	primitiveShader->setUniform("view", camera->getViewMatrix());
	primitiveShader->setUniform("projection", camera->getProjectionMatrix(aspectRatio));

	glDrawArrays(GL_LINES, 0, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}