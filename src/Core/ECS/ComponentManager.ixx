export module ComponentManager;
import Globals;
import std;

namespace ECS {

	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void AddComponentToEntity(Entity entity, T component)
		{
			if (entity_to_index_map.find(entity) == entity_to_index_map.end())
			{
				size_t new_index = component_vector.size();
				entity_to_index_map[entity] = new_index;
				index_to_entity_map[new_index] = entity;
				component_vector.push_back(component);
			}
			else {
				std::cerr << "[ComponentArray::AddComponentToEntity] WARNING -> Attempted to add a component to entity " << entity << " more than once." << std::endl;
			}
		}

		void RemoveComponentFromEntity(Entity entity)
		{
			if (entity_to_index_map.find(entity) != entity_to_index_map.end())
			{
				size_t index_of_removed_entity = entity_to_index_map[entity];
				size_t index_of_last_element = component_vector.size() - 1;
				Entity entity_of_last_element = index_to_entity_map[index_of_last_element];

				std::swap(component_vector[index_of_removed_entity], component_vector[index_of_last_element]);
				entity_to_index_map[entity_of_last_element] = index_of_removed_entity;
				index_to_entity_map[index_of_removed_entity] = entity_of_last_element;

				component_vector.pop_back();
				entity_to_index_map.erase(entity);
				index_to_entity_map.erase(index_of_last_element);
			} 
			else {
				std::cerr << "[ComponentArray::RemoveComponentFromEntity] WARNING -> No component found for entity " << entity << " ." << std::endl;
			}
		}

		T& GetComponentFromEntity(Entity entity)
		{
			if (entity_to_index_map.find(entity) != entity_to_index_map.end())
			{
				return component_vector[entity_to_index_map[entity]];
			}
			else {
				std::cerr << "[ComponentArray::GetComponentFromEntity] ERROR -> No component of " << typeid(T).name() << " for Entity " << entity << "!" << std::endl;
				exit(1);
			}
		}

		void EntityDestroyed(Entity entity) override
		{
			if (entity_to_index_map.find(entity) != entity_to_index_map.end())
			{
				RemoveComponentFromEntity(entity);
			}
		}

		std::vector<Entity> GetEntities() const {
			std::vector<Entity> entities;
			entities.reserve(entity_to_index_map.size());
			for (const auto& pair : entity_to_index_map) {
				entities.push_back(pair.first);
			}
			return entities;
		}

	private:
		std::vector<T> component_vector;
		std::unordered_map<Entity, size_t> entity_to_index_map;
		std::unordered_map<size_t, Entity> index_to_entity_map;
	};

	export class ComponentManager
	{
	public:

		template<typename T>
		void RegisterComponent()
		{
			const char* type_name = typeid(T).name();

			if (component_types.find(type_name) == component_types.end())
			{
				component_types.insert({ type_name, component_type });
				component_arrays.insert({ type_name,  std::make_shared<ComponentArray<T>>() });
				++component_type;
			}
			else {
				std::cerr << "[ComponentManager::RegisterComponent] WARNING -> Component " << type_name << " has already been registered." << std::endl;
			}
		}

		template<typename T>
		ComponentType GetComponentTypeId()
		{
			const char* type_name = typeid(T).name();

			if (component_types.find(type_name) != component_types.end())
			{
				return component_types[type_name];
			}
			else {
				std::cerr << "[ComponentManager::GetComponentTypeId] ERROR ->  Component " << type_name << " was NOT registered." << std::endl;
				exit(1);
			}

		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->AddComponentToEntity(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			GetComponentArray<T>()->RemoveComponentFromEntity(entity);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return GetComponentArray<T>()->GetComponentFromEntity(entity);
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& key_pair : component_arrays)
			{
				auto const& component = key_pair.second;
				component->EntityDestroyed(entity);
			}
		}

		template<typename T>
		std::vector<Entity> GetEntitiesWith() {
			return GetComponentArray<T>()->GetEntities();
		}




	private:
		std::unordered_map<const char*, ComponentType> component_types{};
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> component_arrays{};
		ComponentType component_type = 0;

		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* type_name = typeid(T).name();

			if (component_types.find(type_name) != component_types.end())
			{
				return std::static_pointer_cast<ComponentArray<T>>(component_arrays[type_name]);
			}
			else {
				std::cerr << "[ComponentManager::GetComponentArray] ERROR -> Component " << type_name << " was NOT registered." << std::endl;
				exit(1);
			}
		}

	};

}