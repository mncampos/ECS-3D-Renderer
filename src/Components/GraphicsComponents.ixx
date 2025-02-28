export module GraphicsComponents;
import <GL/glew.h>;
import <glm/glm.hpp>;
import std;

export struct Renderable {

};

export struct Sprite {

};

export struct Camera {
	glm::vec2 position = glm::vec2(32.0f, 32.0f); 
	float zoom = 0.5f;
	float rotation = 0.0f;
	float tilt = 0.0f;
};



export struct Mesh {
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	GLuint texture_id = 0;
	GLuint texture_id2 = 1;
};

