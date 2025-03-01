export module PlayerMovementEvent;
import EventManager;
import Globals;
import <glm/glm.hpp>;

export class PlayerMovementEvent : public ECS::Event
{
public:
	PlayerMovementEvent(glm::ivec3 direction) : direction(direction) {}

	EventType GetType() const override
	{
		return PLAYER_MOVEMENT_EVENT;
	}

	glm::ivec3 GetDirection() const
	{
		return direction;
	}

private:
	glm::ivec3 direction;
};