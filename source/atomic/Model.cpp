#include "Model.h"

using namespace atomic;

ModelAsset::ModelAsset() :
shaders(NULL),
texture(NULL),
vbo(0),
vao(0),
drawType(GL_TRIANGLES),
drawStart(0),
drawCount(0),
shininess(0.0f),
specularColor(1.0f, 1.0f, 1.0f)
{}

ModelInstance::ModelInstance(ModelAsset *asset) : asset(asset), transform()
{}

void ModelInstance::setTransform(glm::mat4 transform)
{
	this->transform = transform;
}

glm::mat4 ModelInstance::getTransform() const
{
	return transform;
}

void ModelInstance::draw(Camera *camera, float aspect)
{
	Program *shaders = asset->shaders;

	shaders->use();
	shaders->setUniform("projection", camera->getProjectionMatrix(aspect));
	shaders->setUniform("view", camera->getViewMatrix());
	shaders->setUniform("model", transform);
	shaders->setUniform("tex", 0);

	shaders->setUniform("material.texture", 0);
	shaders->setUniform("material.shininess", asset->shininess);
	shaders->setUniform("material.specularColor", asset->specularColor);
	shaders->setUniform("material.diffuseColor", asset->diffuseColor);
	shaders->setUniform("light.position", glm::vec3(0,0,0));
	shaders->setUniform("light.color", glm::vec3(1,1,1));
	shaders->setUniform("light.ambient", 0.15f);
	shaders->setUniform("cameraPos", camera->getPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, asset->texture->object());

	glBindVertexArray(asset->vao);
	glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	shaders->stopUsing();
}

ModelAsset *create_box_asset(Program *program)
{
	ModelAsset *box = new ModelAsset();

	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("wooden-crate.jpg");
	bitmap.flipVertically();
	
	box->shaders = program;
	box->drawType = GL_TRIANGLES;
	box->drawStart = 0;
	box->drawCount = 6*2*3;
	box->texture = new Texture(bitmap);
	box->shininess = 80.0f;
	box->specularColor = glm::vec3(1, 1, 1);
	box->diffuseColor = glm::vec3(1, 1, 1);

	glGenVertexArrays(1, &box->vao);
	glGenBuffers(1, &box->vbo);

	glBindVertexArray(box->vao);
	glBindBuffer(GL_ARRAY_BUFFER, box->vbo);

	GLfloat vertexData[] = {
		//  X     Y     Z       U     V  Normal
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// front
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		// left
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		// right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	};

	// test removing normals and texcoords
	/*for (int i = 0; i < 6 * 2 * 3; i++)
	{
		vertexData[i * 8 + 3] = 0.0f;
		vertexData[i * 8 + 4] = 0.0f;
		vertexData[i * 8 + 5] = 0.0f;
		vertexData[i * 8 + 6] = 0.0f;
		vertexData[i * 8 + 7] = 0.0f;
	}*/

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// [0, 1, 2] -> vert
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	// [3, 4] -> vertTexCoord
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	// [5, 6, 7] -> vertNormal
	glEnableVertexAttribArray(program->attrib("vertNormal"));
	glVertexAttribPointer(program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(5 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return box;
}

#include "../lib/tiny_obj_loader.h"

ModelAsset *from_obj(Program *program, char *filePath)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, filePath);

	if (!err.empty())
	{
		fprintf(stderr, "from_obj: %s\n", err);
		return NULL;
	}

	tinyobj::shape_t shape = shapes[0];
	tinyobj::mesh_t mesh = shape.mesh;

	int vertices_per_face = 3;
	int vertices = mesh.indices.size();
	int faces = mesh.indices.size() / vertices_per_face;

	GLfloat vertexData[] = {
		//face1
		 0.5f, -0.5f,  0.0f, 0, 0, 0, 0, 0,
		-0.5f, -0.5f, -0.5f, 0, 0, 0, 0, 0,
		-0.5f, -0.5f,  0.5f, 0, 0, 0, 0, 0,

		//face2
		-0.5f, -0.5f, -0.5f, 0, 0, 0, 0, 0,
		-0.5f, -0.5f,  0.5f, 0, 0, 0, 0, 0,
		 0.0f,  0.5f,  0.5f, 0, 0, 0, 0, 0,

		 //face3
		  0.5f, -0.5f,  0.0f, 0, 0, 0, 0, 0,
		 -0.5f, -0.5f, -0.5f, 0, 0, 0, 0, 0,
		  0.0f,  0.5f,  0.5f, 0, 0, 0, 0, 0,

		 //face4
		  0.5f, -0.5f,  0.0f, 0, 0, 0, 0, 0,
		 -0.5f, -0.5f, -0.5f, 0, 0, 0, 0, 0,
		 -0.5f, -0.5f,  0.5f, 0, 0, 0, 0, 0,
	};

	vertices = 12;
	
	/*GLfloat *vertexData = new GLfloat[vertices * 8];
	printf("vertices: %d  faces: %d\n", vertices, faces);*/

	/*for (int i = 0; i < faces; i++)
	{
		printf(" face[%d] has vertices ", i);

		for (int j = 0; j < vertices_per_face; j++)
		{
			int vertex = vertices_per_face * i + j;
			int index = mesh.indices[vertex];
			printf("%d ", index);

			vertexData[8 * vertex + 0] = mesh.positions[3 * index + 0];
			vertexData[8 * vertex + 1] = mesh.positions[3 * index + 1];
			vertexData[8 * vertex + 2] = mesh.positions[3 * index + 2];
			vertexData[8 * vertex + 3] = 0.0f;
			vertexData[8 * vertex + 4] = 0.0f;
			vertexData[8 * vertex + 5] = 0.0f;
			vertexData[8 * vertex + 6] = 0.0f;
			vertexData[8 * vertex + 7] = 0.0f;
		}

		printf("\n");
	}*/

	ModelAsset *box = new ModelAsset();

	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("wooden-crate.jpg");
	bitmap.flipVertically();

	box->shaders = program;
	box->drawType = GL_TRIANGLES;
	box->drawStart = 0;
	box->drawCount = vertices;
	box->texture = new Texture(bitmap);
	box->shininess = 80.0f;
	box->specularColor = glm::vec3(1, 1, 1);
	box->diffuseColor = glm::vec3(1, 1, 1);

	glGenVertexArrays(1, &box->vao);
	glGenBuffers(1, &box->vbo);

	glBindVertexArray(box->vao);
	glBindBuffer(GL_ARRAY_BUFFER, box->vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// [0, 1, 2] -> vert
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	// [3, 4] -> vertTexCoord
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	// [5, 6, 7] -> vertNormal
	glEnableVertexAttribArray(program->attrib("vertNormal"));
	glVertexAttribPointer(program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(5 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return box;
}

#include <fstream>

ModelAsset *from_bin(Program *program, char *filePath)
{
	std::ifstream is(filePath, std::ifstream::binary);

	if (!is)
		throw std::runtime_error("Failed to open file");

	unsigned int floats;
	is.read((char *)&floats, sizeof(unsigned int));
	GLfloat *vertexData = new GLfloat[floats];
	//GLfloat vertexData[151680];
	is.read((char *)vertexData, sizeof(GLfloat) * floats);
	printf("%f %f %f\n", vertexData[5], vertexData[6], vertexData[7]);
	GLint vertices = floats / 8;
	//printf("%d vertices, %d floats\n", vertices, floats);

	ModelAsset *box = new ModelAsset();

	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("wooden-crate.jpg");
	bitmap.flipVertically();

	box->shaders = program;
	box->drawType = GL_TRIANGLES;
	box->drawStart = 0;
	box->drawCount = vertices;
	box->texture = new Texture(bitmap);
	box->shininess = 80.0f;
	box->specularColor = glm::vec3(1, 1, 1);
	box->diffuseColor = glm::vec3(1, 1, 1);

	glGenVertexArrays(1, &box->vao);
	glGenBuffers(1, &box->vbo);

	glBindVertexArray(box->vao);
	glBindBuffer(GL_ARRAY_BUFFER, box->vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * floats, vertexData, GL_STATIC_DRAW);

	// [0, 1, 2] -> vert
	glEnableVertexAttribArray(program->attrib("vert"));
	glVertexAttribPointer(program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	// [3, 4] -> vertTexCoord
	glEnableVertexAttribArray(program->attrib("vertTexCoord"));
	glVertexAttribPointer(program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));

	// [5, 6, 7] -> vertNormal
	glEnableVertexAttribArray(program->attrib("vertNormal"));
	glVertexAttribPointer(program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid *)(5 * sizeof(GLfloat)));

	glBindVertexArray(0);
	return box;
}