export module GeometryComponents;
import <glm/glm.hpp>;

export struct Transform {
	glm::ivec3 position = glm::ivec3(0, 0, 0);
	glm::ivec3 scale = glm::ivec3(1, 1, 1);
};