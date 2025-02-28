export module MovementSystem;
import SystemManager;
import MovementEvent;
import GeometryComponents;
import <glm/glm.hpp>;
import Globals;
import std;
import Engine;
import WorldComponents;
import GameContext;

namespace ECS {
	export class MovementSystem : public System
	{
	public:

		void Init()
		{
			engine.Subscribe(MOVEMENT_EVENT, [this](const Event& event) {

				});


		}

		void Update(float delta_time) override
		{

		}

	private:
		std::unordered_map<Entity,glm::ivec3> entities_to_move;
	};
}