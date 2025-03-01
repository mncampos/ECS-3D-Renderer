export module Engine;
import SystemManager;
import ComponentManager;
import EntityManager;
import EventManager;
import Globals;
import std;


// Components

import ActorComponent;
import MeshComponent;
import PlayerComponent;
import RenderableComponent;
import TransformComponent;
import HeightmapComponent;
import TerrainMeshComponent;
import TileGridComponent;
import PositionComponent;
import CameraComponent;

namespace ECS {

	export class System {
	public:
		std::set<Entity> Entities;
		virtual void Update(float dt) {}
	};


	export class Engine {
	public:
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static Engine& Get() {
			static Engine instance;
			return instance;
		}

		void Init()
		{
			entity_manager    = std::make_unique<EntityManager>();
			component_manager = std::make_unique<ComponentManager>();
			system_manager    = std::make_unique<SystemManager>();
			event_manager     = std::make_unique<EventManager>();

			Engine::Get().RegisterComponent<Actor>();
			Engine::Get().RegisterComponent<Mesh>();
			Engine::Get().RegisterComponent<Player>();
			Engine::Get().RegisterComponent<Position>();
			Engine::Get().RegisterComponent<Renderable>();
			Engine::Get().RegisterComponent<Transform>();
			Engine::Get().RegisterComponent<Heightmap>();
			Engine::Get().RegisterComponent<TerrainMesh>();
			Engine::Get().RegisterComponent<TileGrid>();
			Engine::Get().RegisterComponent<Camera>();

		}

		Entity createEntity()
		{
			return entity_manager->CreateEntity();
		}

		void DestroyEntity(Entity entity)
		{
			entity_manager		->  DestroyEntity(entity);
			component_manager	->  EntityDestroyed(entity);
			system_manager		->  EntityDestroyed(entity);
		}

		template<typename T>
		void RegisterComponent()
		{
			component_manager	->  RegisterComponent<T>();
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			component_manager->AddComponent(entity, component);

			auto signature = entity_manager->GetSignature(entity);
			signature.set(component_manager->GetComponentTypeId<T>(), true);
			entity_manager->SetSignature(entity, signature);
		    system_manager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		bool HasComponent(Entity entity)
		{
			auto signature = entity_manager->GetSignature(entity);
			return signature.test(component_manager->GetComponentTypeId<T>());
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			component_manager->RemoveComponent<T>(entity);

			auto signature = entity_manager->GetSignature(entity);
			signature.set(component_manager->GetComponentTypeId<T>(), false);
			entity_manager->SetSignature(entity, signature);
			system_manager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return component_manager->GetComponent<T>(entity);
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			return component_manager->GetComponentTypeId<T>();
		}

		template<typename T>
		std::shared_ptr<T> RegisterSystem(int priority = 0)
		{
			static_assert(std::is_base_of_v<System, T>, "T must derive from System");
			auto system = system_manager->RegisterSystem<T>(priority);
			system->Init();
			return system;
		}

		template<typename T>
		void SetSystemSignature(Signature signature)
		{
			system_manager->SetSystemSignature<T>(signature);
		}

		template<typename T>
		std::vector<Entity> GetEntitiesWith() {
			return component_manager->GetEntitiesWith<T>();
		}



		void Subscribe(EventType event_type, EventCallback callback)
		{
			event_manager->Subscribe(event_type, callback);
		}

		void EmitEvent(const std::shared_ptr<Event>& event)
		{
			event_manager->Emit(event);
		}

		void DispatchEvents()
		{
			event_manager->Dispatch();
		}

		void UpdateSystems(float delta_time)
		{
			for (auto&  system : system_manager->GetSystems())
			{
				system->Update(delta_time);
				DispatchEvents();
			}
		}


	private:
		Engine() = default;

		std::unique_ptr<EntityManager> entity_manager;
		std::unique_ptr<ComponentManager>  component_manager;
		std::unique_ptr<SystemManager> system_manager;
		std::unique_ptr<EventManager> event_manager;
	};
}