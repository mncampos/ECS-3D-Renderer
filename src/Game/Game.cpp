module;
#include <SDL3/SDL.h>
#include <chrono>
#include <glm/glm.hpp>
module Game;
import Startup;
import Engine;
import World;
import InputEvents;


Game::Game() : window("Drakensoul", 1280, 768), isRunning(true)
{
    ECS::Engine::Get().Init();
    RegisterSystems();
    ECS::World::Get().Init();

}

void Game::Run()
{

        using clock = std::chrono::high_resolution_clock;
        constexpr std::chrono::duration<float> timestep(0.016f); // 16ms
        std::chrono::duration<float> lag(0.0f);
        auto time_start = clock::now();

        int frame_count = 0;
        float fps_timer = 0.0f;

        while (isRunning)
        {
            auto time_now = clock::now();
            std::chrono::duration<float> delta_time = time_now - time_start;
            time_start = time_now;

            lag += delta_time;

            // FPS counter update
            frame_count++;
            fps_timer += delta_time.count();
            if (fps_timer >= 1.0f) {  // Every second, print FPS
                std::cout << "FPS: " << frame_count << " | Frame Time: "
                    << (1000.0f / frame_count) << " ms" << std::endl;
                frame_count = 0;
                fps_timer = 0.0f;
            }

            while (lag >= timestep) {
                lag -= timestep;
                Update(timestep.count());  // Pass fixed timestep as float
            }

        }
}




void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ECS::Engine::Get().EmitEvent(std::make_shared<InputEvent>(event));
    };
    
}

void Game::Update(float dt)
{
    window.Clear();
    ProcessInput();

    ECS::Engine::Get().DispatchEvents();
    ECS::Engine::Get().UpdateSystems(dt);

    window.SwapBuffers();
}
