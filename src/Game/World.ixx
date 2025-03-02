module;
#include <GL/glew.h>
export module World;
import Globals;
import std;
import <glm/glm.hpp>;
import Engine;
import TransformComponent;
import ShaderManager;
import ModelManager;
import MeshComponent;
import RenderableComponent;
import HeightmapComponent;
import TerrainMeshComponent;
import TileGridComponent;

namespace  ECS {

	struct TerrainData {
		std::vector<float> heights; 
		std::vector<float> vertices; 
		std::vector<unsigned int> indices;
		std::vector<Tile> tiles; 
	};


	export class World {
	public:
		World(const World&) = delete;
		World& operator=(const World&) = delete;

		static World& Get() {
			static World instance;
			return instance;
		}

        float simpleNoise(int x, int y, int seed = 42) {
            int n = x + y * 57 + seed;
            n = (n << 13) ^ n;
            return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) * 0.5f; 
        }

        float getHeight(int x, int y, float scale = 0.1f, float amplitude = 2.0f) {
            return simpleNoise(x, y) * amplitude * scale; 
        }

		void Init()
		{
			constexpr size_t GRID_SIZE = 100;

			GLuint shader = ShaderManager::Get().GetProgram("src\\Rendering\\Shader\\Shaders\\TerrainVertexShader.glsl", "src\\Rendering\\Shader\\Shaders\\TerrainFragmentShader.glsl");

            Entity terrain = Engine::Get().createEntity();
            terrain_entity = terrain;

            // Register static terrain data
            int terrainId = World::Get().RegisterTerrain(GRID_SIZE, GRID_SIZE);
            auto& terrainData = World::Get().GetTerrain(terrainId);

            for (int y = 0; y < GRID_SIZE; ++y) {
                for (int x = 0; x < GRID_SIZE; ++x) {
                    terrainData.heights[y * GRID_SIZE + x] = getHeight(x, y, 0.1f, 2.0f); 
                }
            }

            // Heightmap
            Heightmap heightmap{ GRID_SIZE, GRID_SIZE, terrainId };
            ECS::Engine::Get().AddComponent(terrain, heightmap);

            // TerrainMesh
            TerrainMesh terrainMesh;
            generateTerrainMesh(terrainMesh, terrainData, GRID_SIZE, GRID_SIZE);
            ECS::Engine::Get().AddComponent(terrain, terrainMesh);

            // Renderable
            ECS::Engine::Get().AddComponent(terrain, Renderable{ shader, 0, glm::vec3(.0f, 0.7f, 0.0f) });

            // TileGrid
            TileGrid tileGrid{ GRID_SIZE, GRID_SIZE, terrainId };
            ECS::Engine::Get().AddComponent(terrain, tileGrid);

            // Transform
            ECS::Engine::Get().AddComponent(terrain, Transform{ glm::ivec3(0, 0, 0) });

		}

        int RegisterTerrain(int width, int height) {
            TerrainData data;
            data.heights.resize(width * height, 0.0f);
            data.tiles.resize(width * height);
            int id = (int) terrains.size();
            terrains[id] = std::move(data);
            return id;
        }

         TerrainData& GetTerrain(int id)  {
            return terrains.at(id);
        }

         bool IsOccupied(int x, int y) const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return false;

             const TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             const Tile& tile = terrain_data.tiles[y * tile_grid.width + x];

             return tile.occupant != static_cast<Entity>(-1);
         }

         bool IsWalkable(int  x, int y)  const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return false;

             const TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             const Tile& tile = terrain_data.tiles[y * tile_grid.width + x];

             return tile.isWalkable;
         }

         bool IsEnterable(int x, int  y)  const
         {
             return IsWalkable(x, y) && !IsOccupied(x, y);
         }

         Tile& GetTileAt(int x, int y) const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 throw std::out_of_range("Tile coordinates are out of bounds");

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];
             return tile;
         }

         void SetOccupant(int x, int y, Entity occupant) const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return;

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];
             tile.occupant = occupant;
         }

         void RemoveOccupant(int x, int y) const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return;

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];
             tile.occupant = static_cast<Entity>(-1);
         }

         Entity GetOccupant(int x, int y) const
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return static_cast<Entity>(-1);

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];
             return tile.occupant;
         }

         void MoveOccupant(int x, int y, glm::ivec3 direction) const
         {
             Entity occupant = GetOccupant(x, y);
             int dirX = x + direction.x;
             int dirY = y + direction.y;

             if (IsEnterable(dirX, dirY) && occupant != static_cast<Entity>(-1))
             {
                 SetOccupant(dirX, dirY, occupant);
                 RemoveOccupant(x, y);
             }
         }

         void AddItemToTile(int x, int y, Entity item)
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return;

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];

             if (tile.item_count < 6)
             {
                 tile.items[tile.item_count++] = item;
             }
         }

         void RemoveItemFromTile(int x, int y, Entity item)
         {
             const TileGrid& tile_grid = Engine::Get().GetComponent<TileGrid>(terrain_entity);
             if (x < 0 || x >= tile_grid.width || y < 0 || y >= tile_grid.height)
                 return;

             TerrainData& terrain_data = World::Get().GetTerrain(tile_grid.tile_data_id);
             Tile& tile = terrain_data.tiles[y * tile_grid.width + x];

             for (int i = 0; i < tile.item_count; ++i)
             {
                 if (tile.items[i] == item)
                 {
                     tile.items[i] = tile.items[--tile.item_count];
                     break;
                 }
             }
         }

		
	private:
		World() = default;

        Entity terrain_entity;
		std::unordered_map<int, TerrainData> terrains;

        void generateTerrainMesh(TerrainMesh & mesh, TerrainData & data, int width, int height)
        {
            int vertexWidth = width + 1;
            int vertexHeight = height + 1;
            data.vertices.resize(vertexWidth * vertexHeight * 8);

            for (int y = 0; y < vertexHeight; ++y) {
                for (int x = 0; x < vertexWidth; ++x) {
                    int idx = (y * vertexWidth + x) * 8;
                    int clampedX = std::min(x, width - 1);
                    int clampedY = std::min(y, height - 1);
                    float h = data.heights[clampedY * width + clampedX];
                    data.vertices[idx] = static_cast<float>(x);     
                    data.vertices[idx + 1] = h;                     
                    data.vertices[idx + 2] = static_cast<float>(y); 

                    // Normal calculation with boundary checks
                    float hL = (x > 0) ? data.heights[clampedY * width + std::max(0, x - 1)] : h;
                    float hR = (x < width) ? data.heights[clampedY * width + std::min(width - 1, x + 1)] : h;
                    float hU = (y > 0) ? data.heights[std::max(0, y - 1) * width + clampedX] : h;
                    float hD = (y < height) ? data.heights[std::min(height - 1, y + 1) * width + clampedX] : h;
                    glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hU - hD)); 
                    data.vertices[idx + 3] = normal.x; 
                    data.vertices[idx + 4] = normal.y; 
                    data.vertices[idx + 5] = normal.z; 

                    data.vertices[idx + 6] = static_cast<float>(x) / width;  
                    data.vertices[idx + 7] = static_cast<float>(y) / height; 
                }
            }

            int tileCount = width * height;
            data.indices.resize(tileCount * 6);
            int index = 0;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    int topLeft = y * vertexWidth + x;
                    int topRight = topLeft + 1;
                    int bottomLeft = (y + 1) * vertexWidth + x;
                    int bottomRight = bottomLeft + 1;

                    data.indices[index++] = topLeft;
                    data.indices[index++] = topRight;
                    data.indices[index++] = bottomLeft;
                    data.indices[index++] = topRight;
                    data.indices[index++] = bottomRight;
                    data.indices[index++] = bottomLeft;
                }
            }

            glGenVertexArrays(1, &mesh.VAO);
            glBindVertexArray(mesh.VAO);
            glGenBuffers(1, &mesh.VBO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(float), data.vertices.data(), GL_STATIC_DRAW);
            glGenBuffers(1, &mesh.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
            mesh.vertex_count = vertexWidth * vertexHeight;
            mesh.index_count = tileCount * 6;
        }
	};
}