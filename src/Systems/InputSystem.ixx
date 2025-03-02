export module InputSystem;
import SystemManager;
import <SDL3/SDL.h>;
import Globals;
import std;
import Engine;
import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import InputEvents;
import PlayerComponent;
import PlayerMovementEvent;
import CameraEvents;
import World;
import PositionComponent;
import AttackEvent;

namespace ECS {
    export class InputSystem : public System {
    public:
        void Init() {
            Signature signature;
            signature.set(Engine::Get().GetComponentType<Player>());
            Engine::Get().SetSystemSignature<InputSystem>(signature);

            Engine::Get().Subscribe(SDL_EVENT, [this](const Event& event) {
                const SDL_Event input_event = static_cast<const InputEvent&>(event).GetSDLEvent();
                HandleInput(input_event);
                });
        }


    private:
        void HandleInput(const SDL_Event& sdl_event)
        {
            if (sdl_event.type == SDL_EVENT_KEY_DOWN)
            {

                if (sdl_event.key.key == SDLK_W || SDLK_S || SDLK_A || SDLK_D) {
                    HandleMovement(sdl_event.key.key);
                }

                if (sdl_event.key.key == SDLK_F) {
                    HandleAttack();
                }
                
            }
        }

        void HandleAttack()
        {
            auto& position = Engine::Get().GetComponent<Position>(*Entities.begin());
            auto& tile = World::Get().GetTileAt(position.x - position.facing.x, position.y - position.facing.y);

            std::cout << "Tile  at  " << position.x + position.facing.x << " and " << position.y + position.facing.y << std::endl;

            Engine::Get().EmitEvent(std::make_shared<AttackEvent>(*Entities.begin(), tile));

        }

        void HandleMovement(SDL_Keycode key)
        {
            glm::ivec3 direction(0);

            switch (key) {
            case SDLK_W : 
                direction = glm::ivec3(0, -1, 0);
                break;
            case SDLK_S:
                direction = glm::ivec3(0, 1, 0);
                break;
            case SDLK_A:
                direction = glm::ivec3(-1, 0, 0);
                break;
            case SDLK_D:
                direction = glm::ivec3(1, 0 ,0);
                break;
            default:
                return;
            }

            PlayerStartMovementEvent move(direction);
            Engine::Get().EmitEvent(std::make_shared<PlayerStartMovementEvent>(move));
        }


    };
}
