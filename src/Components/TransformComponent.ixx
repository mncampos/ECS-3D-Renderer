export module TransformComponent;
import <glm/glm.hpp>;

export struct Transform
{
	glm::ivec3 position = glm::ivec3(0);
	glm::vec3 scale = glm::vec3(1);
};