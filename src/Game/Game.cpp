module;
#include <SDL3/SDL.h>
#include <chrono>
module Game;
import <glm/glm.hpp>;
import Engine;
import GeometryComponents;
import ActorComponents;
import GraphicsComponents;
import WorldComponents;
import InputSystem;
import InputEvents;
import Globals;
import World;
import RenderingSystem;

namespace ECS {
    Engine engine;
    World world;
}

Game::Game() : window("Drakensoul", 1280, 768), isRunning(true)
{
    ECS::engine.Init();
    ECS::engine.RegisterComponent<Transform>();
    ECS::engine.RegisterComponent<Mesh>();
    ECS::engine.RegisterComponent<Tile>();
    ECS::engine.RegisterComponent<Renderable>();
    ECS::engine.RegisterComponent<Camera>();
    ECS::engine.RegisterComponent<Player>();


    auto input_system = ECS::engine.RegisterSystem<ECS::InputSystem>();
    {
        Signature signature;
        signature.set(ECS::engine.GetComponentType<Player>());
        ECS::engine.SetSystemSignature<ECS::InputSystem>(signature);
    }
    input_system->Init();


    auto rendering_system = ECS::engine.RegisterSystem<ECS::RenderingSystem>();
    {
        Signature signature;
        signature.set(ECS::engine.GetComponentType<Transform>());
        signature.set(ECS::engine.GetComponentType<Mesh>());
        ECS::engine.SetSystemSignature<ECS::RenderingSystem>(signature);
    }
    rendering_system->Init();


    ECS::world.Init();

   Entity player = ECS::engine.createEntity();
   ECS::engine.AddComponent(player, Player{});
   ECS::engine.AddComponent(player, Transform{ glm::ivec3(0,1,0) });
   ECS::world.OccupyTile(player);

}
void Game::Run()
{
    auto last_time = std::chrono::high_resolution_clock::now();
    float accumulated_time = 0.0f;

    while (isRunning)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        delta_time = std::min(delta_time, 0.1f);
        accumulated_time += delta_time;


        while (accumulated_time >= TARGET_FRAME_TIME)
        {
            Update(TARGET_FRAME_TIME);
            accumulated_time -= TARGET_FRAME_TIME;
        }

        if (delta_time < TARGET_FRAME_TIME)
        {
            float sleep_time = TARGET_FRAME_TIME - delta_time;
            auto sleep_until = current_time + std::chrono::duration<float>(sleep_time);
            std::this_thread::sleep_until(sleep_until);
        }
    }
}


void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ECS::engine.EmitEvent(std::make_shared<InputEvent>(event));
    };
    
}

void Game::Update(float dt)
{
    window.Clear();
    ProcessInput();

    ECS::engine.DispatchEvents();
    ECS::engine.UpdateSystems(dt);

    window.SwapBuffers();
}
