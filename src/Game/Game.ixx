export module Game;
import Window;
import Globals;
import std;


export class Game
{
public:
	Game();
	void Run();

private:
	void ProcessInput();
	void Update(float dt);

	Window window;
	bool isRunning;
};

