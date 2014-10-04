#pragma once

#include <list>
#include "Program.h"
#include "Model.h"

struct GLFWwindow;
class Camera;

namespace atomic
{
	class ClientReactor
	{
		GLFWwindow *window;
		Camera *camera;
		std::list<ModelInstance*> instances;

		bool shouldRun = false;
		bool hadFocus = false;
		float aspectRatio = 1.0f;

		Program *postShader;
		Program *primitiveShader;

		Program *loadShader(const char *vertPath, const char *fragPath);

		void draw();
		void finishDraw();
		void update(double delta);

		GLuint gizmo = 0;
		void drawGizmo();

		GLuint fbo, fbo_texture, rbo_depth;
		GLuint vbo_fbo_vertices;
		GLuint attribute_v_coord_postproc, uniform_fbo_texture;

	public:
		Program *mainShader;
		ClientReactor(const char *title, int width, int height);
		~ClientReactor();

		void addInstance(ModelInstance *instance);
		void removeInstance(ModelInstance *instance);

		void run();
		void stop();
		bool isRunning();
		bool shouldUseInput() { return hadFocus; };

		void framebufferSizeChanged(int width, int height);
	};
};