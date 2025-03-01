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
import Startup;
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
            return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) * 0.5f; // Range: [-0.5, 0.5]
        }

        float getHeight(int x, int y, float scale = 0.1f, float amplitude = 2.0f) {
            return simpleNoise(x, y) * amplitude * scale; // Small elevations (e.g., -1 to 1 units)
        }

		void Init()
		{
			constexpr size_t GRID_SIZE = 100;

			GLuint shader = ShaderManager::Get().GetProgram("src\\Rendering\\Shader\\Shaders\\VertexShader.glsl", "src\\Rendering\\Shader\\Shaders\\FragmentShader.glsl");

            Entity terrain = Engine::Get().createEntity();

            // Register static terrain data
            int terrainId = World::Get().RegisterTerrain(GRID_SIZE, GRID_SIZE);
            auto& terrainData = World::Get().GetTerrain(terrainId);

            for (int y = 0; y < GRID_SIZE; ++y) {
                for (int x = 0; x < GRID_SIZE; ++x) {
                    terrainData.heights[y * GRID_SIZE + x] = getHeight(x, y, 0.4f, 2.0f); // Small hills
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
            int id = terrains.size();
            terrains[id] = std::move(data);
            return id;
        }

         TerrainData& GetTerrain(int id)  {
            return terrains.at(id);
        }
		
	private:
		World() = default;

		std::unordered_map<int, TerrainData> terrains;

        void generateTerrainMesh(TerrainMesh & mesh, TerrainData & data, int width, int height)
        {
            int vertexWidth = width + 1;
            int vertexHeight = height + 1;
            data.vertices.resize(vertexWidth * vertexHeight * 5);

            for (int y = 0; y < vertexHeight; ++y) {
                for (int x = 0; x < vertexWidth; ++x) {
                    int idx = (y * vertexWidth + x) * 5;
                    data.vertices[idx] = static_cast<float>(x);
                    data.vertices[idx + 1] = data.heights[std::min(y, height - 1) * width + std::min(x, width - 1)];
                    data.vertices[idx + 2] = static_cast<float>(y);
                    data.vertices[idx + 3] = static_cast<float>(x) / width;
                    data.vertices[idx + 4] = static_cast<float>(y) / height;
                }
            }

            // Generate indices (static)
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

            // Setup OpenGL buffers (static)
            glGenVertexArrays(1, &mesh.VAO);
            glBindVertexArray(mesh.VAO);
            glGenBuffers(1, &mesh.VBO);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(float), data.vertices.data(), GL_STATIC_DRAW);
            glGenBuffers(1, &mesh.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
            mesh.vertex_count = vertexWidth * vertexHeight;
            mesh.index_count = tileCount * 6;
        }
	};
}