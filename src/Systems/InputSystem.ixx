export module InputSystem;
import SystemManager;
import <SDL3/SDL.h>;
import Globals;
import std;
import Engine;
import InputEvents;


namespace ECS {
	export class InputSystem : public System
	{
	public:
		void Init()
		{
			engine.Subscribe(SDL_EVENT, [this](const Event& event) {
				const SDL_Event input_event = static_cast<const InputEvent&>(event).GetSDLEvent();

				switch (input_event.type)
				{
				case SDL_EVENT_KEY_DOWN:
					switch (input_event.key.key)
					{
					case SDLK_W: std::cout << "Pressed W" << std::endl; break;
					case SDLK_S: std::cout << "Pressed S" << std::endl; break;
					case SDLK_A: break;
					case SDLK_D: break;
					}
					break;

				case SDL_EVENT_QUIT: exit(1); break;

				default: break;
				}

				});
		}
	};
}