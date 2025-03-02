module;
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
export module CameraSystem;
import Engine;
import PlayerMovementEvent;
import SystemManager;
import PlayerComponent;
import CameraComponent;
import PositionComponent;
import CameraEvents;

namespace ECS {
	export class CameraSystem : public System {
	public:
		void Init()
		{
			Signature signature;
			signature.set(Engine::Get().GetComponentType<Player>());
			Engine::Get().SetSystemSignature<CameraSystem>(signature);


			Engine::Get().Subscribe(PLAYER_FINISHED_MOVEMENT_EVENT, [this](const Event& event) {
				shouldUpdate = true;
				});

		}

		void Update(float dt) override
		{
			if (!shouldUpdate) {
				return;
			}

			auto& camera = Engine::Get().GetComponent<Camera>(*Entities.begin());



			for (const auto& player : Entities)
			{
				auto& pos = Engine::Get().GetComponent<Position>(player);

				std::cout << "Player Pos(" << pos.x << "," << pos.y << "," << pos.z << ")" << std::endl;

				camera.position = glm::vec3(static_cast<float>(pos.x), 10.0f, static_cast<float>(pos.y) + 5);
				camera.view = glm::lookAt(
					camera.position,
					glm::vec3(pos.x, pos.z, pos.y),
					glm::vec3(0, 1, 0)
				);
			}
			shouldUpdate = false;
			
			CameraUpdateEvent event(camera.view, camera.projection, camera.position);
			Engine::Get().EmitEvent(std::make_shared<CameraUpdateEvent>(event));
		}


	private:
		bool shouldUpdate = true;
	};
}