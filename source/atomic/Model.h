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

		void setPosition(glm::vec3 position);
		void setRotation(glm::mat4 rotation);
		void setScale(glm::vec3 scale);

		glm::vec3 getPosition() const;
		glm::mat4 getRotation() const;
		glm::vec3 getScale() const;

		glm::mat4 getModelMatrix() const;

		void draw(Camera *camera, float aspect);

	private:
		ModelAsset* asset;

		glm::vec3 position;
		glm::mat4 rotation;
		glm::vec3 scale;		
	};
};

atomic::ModelAsset *create_box_asset(atomic::Program *shaders);
atomic::ModelAsset *from_bin(atomic::Program *shaders, char *filePath);