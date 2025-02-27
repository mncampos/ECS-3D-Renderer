module;
#include <SDL3/SDL.h>
module Game;


Game::Game() : window("Drakensoul", 1280, 768), isRunning(true)
{

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
    }
}

void Game::Update(float dt)
{
    window.Clear();
    ProcessInput();

    window.SwapBuffers();
}
