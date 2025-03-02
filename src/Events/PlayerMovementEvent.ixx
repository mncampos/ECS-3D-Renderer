export module PlayerMovementEvent;
import EventManager;
import Globals;
import <glm/glm.hpp>;

export class PlayerStartMovementEvent : public ECS::Event
{
public:
	PlayerStartMovementEvent(glm::ivec3 direction) : direction(direction) {}

	EventType GetType() const override
	{
		return PLAYER_START_MOVEMENT_EVENT;
	}

	glm::ivec3 GetDirection() const
	{
		return direction;
	}

private:
	glm::ivec3 direction;
};

export class PlayerFinishedMovementEvent : public ECS::Event
{
public:
	PlayerFinishedMovementEvent() {}

	EventType GetType() const override
	{
		return PLAYER_FINISHED_MOVEMENT_EVENT;
	}

};

