export module InputEvents;
import EventManager;
import Globals;
import <SDL3/SDL.h>;
import std;

export class InputEvent : public ECS::Event {
public:
	InputEvent(const SDL_Event& sdl_event) : SDL_event(sdl_event) {};

	EventType GetType() const override
	{
		return SDL_EVENT;
	}

	const SDL_Event& GetSDLEvent() const
	{
		return SDL_event;
	}

	uint32_t GetSDLEventType() const
	{
		return SDL_event.type;
	}




private:
	SDL_Event SDL_event;

};