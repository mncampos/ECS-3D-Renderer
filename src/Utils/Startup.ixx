module;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <stb_image.h>
#include <string>
#include <iostream>
export module Startup;
import Engine;
import RenderingSystem;
import InputSystem;
import MovementSystem;
import ShaderManager;
import ModelManager;
import MeshComponent;
import TransformComponent;
import RenderableComponent;
import TerrainRenderingSystem;
import CameraSystem;
import PlayerComponent;
import PositionComponent;
import PlayerMovementSystem;
import World;


export void RegisterSystems()
{
	using namespace ECS;

    Engine::Get().RegisterSystem<InputSystem>(1);
    Engine::Get().RegisterSystem<PlayerMovementSystem>(2);
    Engine::Get().RegisterSystem<MovementSystem>(3);
    Engine::Get().RegisterSystem<CameraSystem>(4);
    Engine::Get().RegisterSystem<RenderingSystem>(5);
	Engine::Get().RegisterSystem<TerrainRenderingSystem>(6);
}

export void AddPlayer()
{
    using namespace ECS;
    GLuint shader = ShaderManager::Get().GetProgram("src\\Rendering\\Shader\\Shaders\\TerrainVertexShader.glsl", "src\\Rendering\\Shader\\Shaders\\TerrainFragmentShader.glsl");

    auto manModel = ModelManager::Get().GetModel(".\\Assets\\Models\\man.obj");
    std::cout << "man.obj: vertex_count = " << manModel->vertex_count << ", index_count = " << manModel->index_count << std::endl;

    Entity player = ECS::Engine::Get().createEntity();

    ECS::Engine::Get().AddComponent(player, Player{});
    ECS::Engine::Get().AddComponent(player, Position{10,10, 0});
    ECS::Engine::Get().AddComponent(player, Mesh{ manModel });
    ECS::Engine::Get().AddComponent(player, Renderable{ shader, 0, glm::vec3(1.0f, 0.0f, 0.0f) });

    World::Get().SetOccupant(10, 10, player);
    World::Get().SetOccupant(11, 10, 123);

}


export void LoadModels()
{
	GLuint shader = ShaderManager::Get().GetProgram("src\\Rendering\\Shader\\Shaders\\TerrainVertexShader.glsl", "src\\Rendering\\Shader\\Shaders\\TerrainFragmentShader.glsl");
	Entity cube = ECS::Engine::Get().createEntity();
	ECS::Engine::Get().AddComponent(cube, Mesh{ ModelManager::Get().GetModel(".\\Assets\\Models\\cube_cube.obj") });
	ECS::Engine::Get().AddComponent(cube, Transform{ glm::ivec3(1,0,0) });
	ECS::Engine::Get().AddComponent(cube, Renderable{ shader, 0, glm::vec3(1.0f, 0.5f, 0.2f) });


}

export GLuint LoadTexture(const std::string& path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Pixel art look
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Pixel art look

    int width, height, nrChannels;

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << " - " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
    return textureID;

}