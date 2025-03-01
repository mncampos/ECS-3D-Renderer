export module MovementSystem;
import SystemManager;
import MovementEvent;
import <glm/glm.hpp>;
import Globals;
import std;
import ActorComponent;
import Engine;
import TransformComponent;

namespace ECS {
	export class MovementSystem : public System
	{
	public:

		void Init()
		{
			Signature signature;
			signature.set(Engine::Get().GetComponentType<Actor>());
			signature.set(Engine::Get().GetComponentType<Transform>());
			Engine::Get().SetSystemSignature<MovementSystem>(signature);

			Engine::Get().Subscribe(MOVEMENT_EVENT, [this](const Event& event) {

				});


		}

		void Update(float delta_time) override
		{

		}

	private:
		std::unordered_map<Entity,glm::ivec3> entities_to_move;
	};
}