#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
	glGenTextures(1, &fbo_color);
	glBindTexture(GL_TEXTURE_2D, fbo_color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Depth buffer
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fbo_depth);
	glBindTexture(GL_TEXTURE_2D, fbo_depth);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);
	
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

	glGenBuffers(1, &screen_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// test
	font = new FontFace("test.ttf", 0);
}

ClientReactor::~ClientReactor()
{
	glDeleteBuffers(1, &screen_vbo);
	glDeleteTextures(1, &fbo_depth);
	glDeleteTextures(1, &fbo_color);
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
		
		draw(delta);
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

	glBindTexture(GL_TEXTURE_2D, fbo_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, fbo_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

	glViewport(0, 0, width, height);
}

void ClientReactor::draw(float delta)
{
	double start_draw = glfwGetTime();
	glfwMakeContextCurrent(window);

	// Draw world into fbo
	double start_scene = glfwGetTime();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	double start_clear = glfwGetTime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double time_clear = glfwGetTime() - start_clear;

	std::vector<ModelInstance*>::iterator it;

	for (it = instances.begin(); it != instances.end(); ++it)
		(*it)->draw(camera, aspectRatio, (float)glfwGetTime());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	double time_scene = glfwGetTime() - start_scene;

	// Draw fbo to screen with postproc
	start_clear = glfwGetTime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	time_clear += glfwGetTime() - start_clear;
	double start_post = glfwGetTime();
	postShader->use();

	postShader->assignTexture("colorTex", 0, fbo_color);
	postShader->assignTexture("depthTex", 1, fbo_depth);
	
	postShader->setUniform("time", glfwGetTime());
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	double time_post = glfwGetTime() - start_post;

	// Extra
	drawGizmo();

	//font->setSize(0, 16);
	//font->render("hello world", -1 + sx * 8, 1 - 50 * sy, sx, sy);
	//font->render("hello world", 0.0f, 0.0f, sx, sy, camera->getProjectionMatrix(aspectRatio) * camera->getViewMatrix());
	//font->render(str, -1 + sx * 8, 1 - 50 * sy, sx, sy, glm::mat4());

	double start_text = glfwGetTime();
	drawDebugText("mspf  ", delta, 0);
	drawDebugText("clear ", time_clear, 24);
	drawDebugText("scene ", time_scene, 36);
	drawDebugText("post  ", time_post, 48);
	double time_text = glfwGetTime() - start_text;
	drawDebugText("^text ", time_text, 60);
}

void ClientReactor::drawDebugText(const char *prefix, double value, int offset)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float sx = 2.0f / width;
	float sy = 2.0f / height;

	std::stringstream fps;
	fps << prefix;
	fps << value;
	const char *str = strdup(fps.str().c_str());

	font->setSize(0, 10);
	font->render(str, -1 + sx * 8, 1 - 12 * sy - offset * sy, sx, sy, glm::mat4());
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