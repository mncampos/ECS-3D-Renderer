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
    AddPlayer();



   

}

void Game::Run()
{

        using clock = std::chrono::high_resolution_clock;
        constexpr std::chrono::duration<float> timestep(0.016f); 
        std::chrono::duration<float> lag(0.0f);
        auto time_start = clock::now();

        while (isRunning)
        {
            auto time_now = clock::now();
            std::chrono::duration<float> delta_time = time_now - time_start;
            time_start = time_now;

            lag += delta_time;


            while (lag >= timestep) {
                lag -= timestep;
                Update(timestep.count());  
            }

        }
}




void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            ECS::Engine::Get().EmitEvent(std::make_shared<InputEvent>(event));
        }
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
