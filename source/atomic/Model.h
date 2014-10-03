#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Program.h"
#include "Texture.h"
#include "Camera.h"

namespace atomic
{
	class ModelAsset
	{
	public:
		Program* shaders;
		Texture* texture;
		GLuint vbo;
		GLuint vao;
		GLenum drawType;
		GLint drawStart;
		GLint drawCount;
		GLfloat shininess;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;

		ModelAsset();
	};

	class ModelInstance
	{
	public:
		ModelInstance(ModelAsset *asset);

		void setTransform(glm::mat4 transform);
		glm::mat4 getTransform() const;

		void draw(Camera *camera, float aspect);

	private:
		ModelAsset* asset;
		glm::mat4 transform;
	};
};

atomic::ModelAsset *create_box_asset(atomic::Program *shaders);
atomic::ModelAsset *from_obj(atomic::Program *shaders, char *filePath);
atomic::ModelAsset *from_bin(atomic::Program *shaders, char *filePath);