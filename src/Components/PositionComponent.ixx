export module PositionComponent;
import <glm/glm.hpp>;

export struct Position {
	int x, y = 1;
	float z = 2;
	glm::ivec2 facing = glm::ivec2(1, 0);
};