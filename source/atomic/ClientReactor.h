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
	public:
		Camera *camera;

		ClientReactor(const char *title, int width, int height);
		~ClientReactor();

		void addInstance(ModelInstance *instance);
		void removeInstance(ModelInstance *instance);

		void run();
		void stop();
		
		void framebufferSizeChanged(int width, int height);

	private:
		GLFWwindow *window;
		std::vector<ModelInstance*> instances;

		float aspectRatio = 1.0f;

		Program *postShader;
		Program *primitiveShader;

		GLuint fbo, fbo_color, fbo_depth;
		GLuint screen_vbo;
		
		void draw();
		void finishDraw();
		void update(double delta, bool hadFocus);

		GLuint gizmo = 0;
		void drawGizmo();
	};
};