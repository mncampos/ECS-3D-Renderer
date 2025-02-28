export module SystemManager;
import Globals;
import std;

namespace ECS {
	export class System {
	public:
		std::set<Entity> Entities;
		virtual void Update(float dt) {}
	};

	export class SystemManager {
	public:

		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			const char* type_name = typeid(T).name();

			if (systems.find(type_name) == systems.end())
			{
				auto system = std::make_shared<T>();
				systems.insert({ type_name, system });
				return system;
			}
			else {
				std::cerr << "[SystemManager::RegisterSystem] ERROR -> System " << type_name << " already registered." << std::endl;
				exit(1);
			}
		
		}

		template<typename T>
		void SetSystemSignature(Signature signature)
		{
			const char* type_name = typeid(T).name();

			if (systems.find(type_name) != systems.end())
			{
				signatures.insert({ type_name, signature });
			}
			else {
				std::cerr << "[SystemManager::SetSignature] ERROR -> System " << type_name << " is not registered." << std::endl;
				exit(1);
			}
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& key_pair : systems)
			{
				auto const& system = key_pair.second;
				system->Entities.erase(entity);
			}
		}

		void EntitySignatureChanged(Entity entity, Signature entity_signature)
		{
			for (auto const& pair : systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& system_signature = signatures[type];

				if ((entity_signature & system_signature) == system_signature)
				{
					system->Entities.insert(entity);
				}
				else {
					if (system->Entities.contains(entity))
					{
					system->Entities.erase(entity);
					}
				}
			}
		}

		void UpdateSystems(float delta_time)
		{
			for (auto& [type, system] : systems)
			{
				system->Update(delta_time);
			}
		}



	private:
		std::unordered_map<const char*, std::shared_ptr<System>> systems{};
		std::unordered_map<const char*, Signature> signatures{};
	};
}