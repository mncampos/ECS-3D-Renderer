export module World;
import Globals;
import std;
import WorldComponents;
import <glm/glm.hpp>;
import GeometryComponents;
import Engine;

namespace  ECS {
	export class World {
	public:

		void Init()
		{
			constexpr size_t GRID_SIZE = 64;

			for(int i = 0; i < GRID_SIZE; i++)
				for(int j = 0; j < GRID_SIZE; j++)
					for (int k = 0; k < 7; k++)
					{
						Entity new_entity = engine.createEntity();
						engine.AddComponent(new_entity, Tile{ TileType::Grass });
						engine.AddComponent(new_entity, Transform{ glm::ivec3(i,j,k) });
						AddTile(new_entity, glm::ivec3(i, j, k));
					}
		}


		void AddTile(Entity entity, const glm::ivec3& position)
		{
			tile_map[position] = entity;
		}

		Tile& GetTileAt(const glm::ivec3& position) const
		{
			auto it = tile_map.find(position);
			return engine.GetComponent<Tile>(it->second);
		}

		Entity GetTileEntityAt(const glm::ivec3& position) const
		{
			auto it = tile_map.find(position);
			return it->second;
		}

		Tile& OccupyTile(Entity entity)
		{
			auto& transform = engine.GetComponent<Transform>(entity);
			auto it = tile_map.find(transform.position);
			auto& component = engine.GetComponent<Tile>(it->second);
			if (component.occupied == static_cast<Entity>(-1))
			{
				component.occupied = entity;
			}
			return component;
		}
		
	private:
		struct PositionHash {
			size_t operator()(const glm::ivec2& pos) const {
				return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
			}
		};
		std::unordered_map<glm::ivec3, Entity, PositionHash> tile_map;

	};
}