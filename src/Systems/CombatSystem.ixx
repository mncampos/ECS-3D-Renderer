module;
#include <glm/glm.hpp>
export module CombatSystem;
import SystemManager;
import Engine;
import World;
import ActorComponent;

import PositionComponent;
import AttributesComponent;
import HealthComponent;
import AttackEvent;
namespace  ECS {
	export class CombatSystem : public System {
	public:
		void Init()
		{
			Signature signature;
			signature.set(Engine::Get().GetComponentType<Actor>());
			signature.set(Engine::Get().GetComponentType<Health>());
			signature.set(Engine::Get().GetComponentType<Attributes>());
			signature.set(Engine::Get().GetComponentType<Position>());
			Engine::Get().SetSystemSignature<CombatSystem>(signature);

			Engine::Get().Subscribe(ATTACK_EVENT, [this](const Event& event) {
				const auto& attack_event = static_cast<const AttackEvent&>(event);
				HandleAttack(attack_event);
				});
			

		}

	private:
		void HandleAttack(AttackEvent event)
		{
			Entity target = event.GetTile().occupant;
			
			Entity attacker = event.GetAttacker();


			if (target != static_cast<Entity>(-1))
			{
			std::cout << Engine::Get().GetComponent<Actor>(attacker).Name << " is trying to attack " << Engine::Get().GetComponent<Actor>(target).Name << "!" << std::endl;
				auto& target_health = Engine::Get().GetComponent<Health>(target);
				auto& attacker_str = Engine::Get().GetComponent<Attributes>(attacker).strength;

				unsigned int damage = attacker_str * 4;
				target_health.current_health -= damage;

				std::cout << "Attack landed and dealt " << damage << " points of damage!" << std::endl;
				std::cout << Engine::Get().GetComponent<Actor>(target).Name << " health is now " << target_health.current_health << "." << std::endl;

				if (target_health.current_health <= 0)
				{
					std::cout << "Target died like a dog!" << std::endl;
					std::cout << "ATTACKER(PLAYER) IS ENTITY " << attacker << " THE DEAD IS " << target << std::endl;
					Engine::Get().DestroyEntity(target);
					event.GetTile().occupant = static_cast<Entity>(-1);
				}

			}
			else {
				std::cout << Engine::Get().GetComponent<Actor>(attacker).Name << " attacked the air! " << std::endl;
			}
		}
	};
}