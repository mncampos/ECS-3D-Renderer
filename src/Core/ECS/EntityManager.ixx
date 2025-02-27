export module EntityManager;
import Globals;
import std;

namespace ECS {
	export class EntityManager
	{
	public:

		EntityManager() : ID(0) {};

		Entity CreateEntity()
		{
			Entity new_entity = 0;

			if (free_entities.empty())
			{
				new_entity = ID++;
			}
			else {
				new_entity = free_entities.front();
				free_entities.pop();
			}

			active_entities.push_back(new_entity);
			entity_to_index_map[new_entity] = active_entities.size() - 1;
			entity_to_signature_map[new_entity] = Signature{};

			return new_entity;

		}

		void DestroyEntity(Entity entity)
		{
			auto it = entity_to_index_map.find(entity);

			if (it != entity_to_index_map.end())
			{
				size_t target_index = it->second;

				std::swap(active_entities[target_index], active_entities.back());
				entity_to_index_map[active_entities[target_index]] = target_index;
				active_entities.pop_back();
				entity_to_index_map.erase(it);
				entity_to_signature_map.erase(entity);
				free_entities.push(entity);
			}
			else {
				std::cerr << "[EntityManager::DestroyEntity] WARNING -> Entity " << entity << " does not exist or already been destroyed." << std::endl;
			}
		}

		Signature GetSignature(Entity entity) const
		{
			auto it = this->entity_to_signature_map.find(entity);

			if (it != entity_to_signature_map.end())
			{
				return it->second;
			}
			else {
				std::cerr << "[EntityManager::GetSignature] ERROR -> Attempted to get signature of an inactive entity" << std::endl;
				exit(1);
			}
		}

		void SetSignature(Entity entity, Signature signature)
		{
			auto it = entity_to_signature_map.find(entity);

			if (it != entity_to_signature_map.end())
			{
				it->second = signature;
			}
			else {
				std::cerr << "[EntityManager::GetSignature] WARNING -> Attempted to set signature of an inactive entity" << std::endl;
			}
		}


	private:

		Entity ID = 0;
		std::vector<Entity> active_entities;
		std::queue<Entity> free_entities;
		std::unordered_map<Entity, size_t> entity_to_index_map;
		std::unordered_map<Entity, Signature> entity_to_signature_map;
	};
}