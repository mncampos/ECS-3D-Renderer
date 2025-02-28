export module MovementEvent;
import EventManager;
import Globals;
import GeometryComponents;
import <glm/glm.hpp>;

export class MovementEvent : public ECS::Event
{
public:
	MovementEvent(glm::ivec3 direction, Entity entity) : direction(direction), entity(entity) {}

	EventType GetType() const override
	{
		return MOVEMENT_EVENT;
	}

	 Entity GetEntity() const
	{
		return entity;
	}

	 glm::ivec3 GetDirection() const
	{
		return direction;
	}
	
private:
	Entity entity;
	glm::ivec3 direction;
};