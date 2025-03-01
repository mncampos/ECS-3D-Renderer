module;
#include <vector>
#include <GL/glew.h>
export module TerrainMeshComponent;


export struct TerrainMesh {
	GLuint VAO, VBO, EBO;
	int vertex_count;
	int index_count;
};