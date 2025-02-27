module;
#include <SDL3/SDL.h>
export module Window;
import std;

export class Window
{
public:
	Window(const std::string& title, int width, int  height);
	~Window();
	void Clear();
	void SwapBuffers();
	bool ShouldClose() const;

private:
	SDL_Window* window;
	SDL_GLContext gl_context;
	bool isOpen;
};