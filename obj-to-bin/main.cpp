#include <fstream>
#include "tiny_obj_loader.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <obj file to convert>\nConverts to Atomic bin format\n", argv[0]);
		return 1;
	}

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	printf("Loading \"%s\"\n", argv[1]);
	std::string error = tinyobj::LoadObj(shapes, materials, argv[1]);

	if (!error.empty())
	{
		printf("Failed to load obj file: %s\n", error);
		return 1;
	}

	printf("Converting \"%s\"\n", argv[1]);
	printf(" - Shapes: %d\n", shapes.size());
	printf(" - Materials: %d\n", materials.size());
	printf("\n");

	std::ofstream s("model.bin", std::ios::binary);

	unsigned int vertices = 0;

	for (size_t i = 0; i < shapes.size(); i++)
	{
		vertices += shapes[i].mesh.indices.size() / 3;
	}
	
	printf("Contains %d vertices\n", vertices);

	unsigned int floats = vertices * 8 * 3;
	s.write((char *)&floats, sizeof(unsigned int));

	float *data = new float[floats];
	printf("Allocated vertex buffer\n");

	unsigned int current = 0;

	for (size_t i = 0; i < shapes.size(); i++)
	{
		tinyobj::mesh_t mesh = shapes[i].mesh;

		bool has_normals = !mesh.normals.empty();
		bool has_texcoords = !mesh.texcoords.empty();
		
		if (has_normals) printf("shape %d has normals\n", i);
		else printf("shape %d does not have normals\n", i);

		if (has_texcoords) printf("shape %d has texcoords\n", i);
		else printf("shape %d does not have texcoords\n", i);

		for (size_t j = 0; j < mesh.indices.size() / 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				int ind = mesh.indices[3 * j + k];

				data[current++] = mesh.positions[3 * ind];
				data[current++] = mesh.positions[3 * ind + 1];
				data[current++] = mesh.positions[3 * ind + 2];

				if (has_texcoords)
				{
					data[current++] = mesh.texcoords[2 * ind];
					data[current++] = mesh.texcoords[2 * ind + 1];
				}
				else
				{
					data[current++] = 0.0f;
					data[current++] = 0.0f;
				}

				if (has_normals)
				{
					data[current++] = mesh.normals[3 * ind];
					data[current++] = mesh.normals[3 * ind + 1];
					data[current++] = mesh.normals[3 * ind + 2];
				}
				else
				{
					data[current++] = 0.0f;
					data[current++] = 0.0f;
					data[current++] = 0.0f;
				}
			}
		}
	}

	printf("Flushing to file\n");

	s.write((char *)data, sizeof(float) * floats);
	s.close();

	return 0;
}