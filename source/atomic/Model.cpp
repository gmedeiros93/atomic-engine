#include <fstream>
#include "Model.h"
#include <GLFW/glfw3.h>
#include "../exceptions.h"

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

ModelInstance::ModelInstance(ModelAsset *asset) : asset(asset),
position(),
rotation(),
scale(1.0f)
{}

void ModelInstance::setPosition(glm::vec3 position) { this->position = position; }
void ModelInstance::setRotation(glm::mat4 rotation) { this->rotation = rotation; }
void ModelInstance::setScale(glm::vec3 scale) { this->scale = scale; }

glm::vec3 ModelInstance::getPosition() const { return position; }
glm::mat4 ModelInstance::getRotation() const { return rotation; }
glm::vec3 ModelInstance::getScale() const { return scale; }

glm::mat4 ModelInstance::getModelMatrix() const
{
	glm::mat4 matrix = glm::translate(glm::mat4(), position);

	matrix *= glm::scale(glm::mat4(), scale);
	matrix *= rotation;

	return matrix;
}

void ModelInstance::draw(Camera *camera, float aspect, float time)
{
	Program *shaders = asset->shaders;

	shaders->use();
	shaders->setUniform("projection", camera->getProjectionMatrix(aspect));
	shaders->setUniform("view", camera->getViewMatrix());
	shaders->setUniform("model", getModelMatrix());
	
	shaders->setUniform("material.texture", 1);
	shaders->setUniform("material.shininess", asset->shininess);
	shaders->setUniform("material.specularColor", asset->specularColor);
	shaders->setUniform("material.diffuseColor", asset->diffuseColor);
	//shaders->setUniform("light.position", glm::vec3(0,0,0));
	//shaders->setUniform("light.position", camera->getPosition());
	//shaders->setUniform("light.color", glm::vec3(1,1,1));
	//shaders->setUniform("light.ambient", 0.000f);
	//shaders->setUniform("light.attenuation", 0.2f);
	/*shaders->setUniform("totalDirLights", 0);
	shaders->setUniform("totalPointLights", 1);
	shaders->setUniform("totalSpotLights", 0);
	shaders->setUniform("pointLights[0].position", camera->getPosition());
	shaders->setUniform("pointLights[0].diffuse", glm::vec3(1.0f));
	shaders->setUniform("pointLights[0].specular", glm::vec3(1.0f));
	shaders->setUniform("pointLights[0].linearFalloff", 0.35);
	shaders->setUniform("pointLights[0].quadraticFalloff", 0.44);*/
	shaders->setUniform("cameraPos", camera->getPosition());

	std::vector<Light> lights;

	Light playerLight;
	playerLight.type = POINT_LIGHT;
	playerLight.vec = camera->getPosition();
	playerLight.diffuse = glm::vec3(1.0f);
	playerLight.specular = glm::vec3(1.0f);
	playerLight.linear = 0.35f;
	playerLight.quadratic = 0.44f;

	lights.push_back(playerLight);

	shaders->setUniform("totalLights", (int)lights.size());

	for (size_t i = 0; i < lights.size(); i++)
	{
		shaders->setUniform(lightUniformName(i, "type"), lights[i].type);
		shaders->setUniform(lightUniformName(i, "vec"), lights[i].vec);
		shaders->setUniform(lightUniformName(i, "ambient"), lights[i].ambient);
		shaders->setUniform(lightUniformName(i, "diffuse"), lights[i].diffuse);
		shaders->setUniform(lightUniformName(i, "specular"), lights[i].specular);
		shaders->setUniform(lightUniformName(i, "diffuse"), lights[i].diffuse);
		shaders->setUniform(lightUniformName(i, "linear"), lights[i].linear);
		shaders->setUniform(lightUniformName(i, "quadratic"), lights[i].quadratic);
	}
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, asset->texture->object());

	glBindVertexArray(asset->vao);
	glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}

/*ModelAsset *create_box_asset(Program *program)
{
	ModelAsset *box = new ModelAsset();

	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("textures/wooden-crate.jpg");
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
}*/

ModelAsset *atomic::from_bin(char *filePath)
{
	std::ifstream is(filePath, std::ifstream::binary);

	if (!is)
		throw std::runtime_error("Failed to open file");

	double load_start = glfwGetTime();

	unsigned int floats;
	is.read((char *)&floats, sizeof(unsigned int));
	GLfloat *vertexData = new GLfloat[floats];
	is.read((char *)vertexData, sizeof(GLfloat) * floats);

	double load_total = glfwGetTime() - load_start;
	GLint vertices = floats / 8;

	printf("Loaded %d vertices from \"%s\" in %f seconds\n", vertices, filePath, load_total);
	ModelAsset *box = new ModelAsset();

	//atomic::Bitmap bitmap = atomic::Bitmap::fromFile("wooden-crate.jpg");
	atomic::Bitmap bitmap = atomic::Bitmap::fromFile("textures/lost_empire-RGBA.png");
	bitmap.flipVertically();

	atomic::Program *program = atomic::Program::fromCommonFiles("shaders/main_vert.glsl", "shaders/main_frag.glsl");

	box->shaders = program;
	box->drawType = GL_TRIANGLES;
	box->drawStart = 0;
	box->drawCount = vertices;
	box->texture = new Texture(bitmap);
	box->shininess = 120.0f;
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