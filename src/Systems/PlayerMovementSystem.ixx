module;
#include <glm/glm.hpp>
export module PlayerMovementSystem;
import SystemManager;
import PlayerMovementEvent;
import Engine;
import PositionComponent;
import World;
import PlayerComponent;

namespace ECS {
	export class PlayerMovementSystem : public System
	{
	public:
		void Init()
		{
			Signature signature;
			signature.set(Engine::Get().GetComponentType<Player>());
			Engine::Get().SetSystemSignature<PlayerMovementSystem>(signature);

			Engine::Get().Subscribe(PLAYER_START_MOVEMENT_EVENT, [this](const Event& event) {
				const PlayerStartMovementEvent move_event = static_cast<const PlayerStartMovementEvent&>(event);
                HandleMovement(move_event);
				});
		}


	private:
        void HandleMovement(PlayerStartMovementEvent move_event)
        {
            auto& pos_component = Engine::Get().GetComponent<Position>(*Entities.begin());
			glm::ivec3 pos = move_event.GetDirection();


			if (World::Get().IsEnterable(pos_component.x + pos.x, pos_component.y + pos.y)) {
				World::Get().MoveOccupant(pos_component.x, pos_component.y, pos);
				pos_component.x += pos.x;
				pos_component.y += pos.y;
				pos_component.z += pos.z;

				pos_component.facing.x = -pos.x;
				pos_component.facing.y = -pos.y;

				Engine::Get().EmitEvent(std::make_shared<PlayerFinishedMovementEvent>());
			
			}
			else {
				std::cout << "Not enterable" << std::endl;
			}
        }
	};
}