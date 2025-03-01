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
import CameraEvents;

namespace ECS {
    export class InputSystem : public System {
    public:
        void Init() {
            Signature signature;
            signature.set(Engine::Get().GetComponentType<Player>());
            Engine::Get().SetSystemSignature<InputSystem>(signature);

            Engine::Get().Subscribe(SDL_EVENT, [this](const Event& event) {
                const SDL_Event input_event = static_cast<const InputEvent&>(event).GetSDLEvent();
                ProcessInputEvent(input_event);
                });
        }

        void Update(float deltaTime) {
            ProcessContinuousMovement(deltaTime);
            ProcessMouseMovement();
        }

    private:
        std::unordered_set<SDL_Keycode> activeKeys;
        glm::vec2 mouseDelta = glm::vec2(0.0f);
        glm::vec2 prevMouseDelta = glm::vec2(0.0f);
        const float mouseSmoothFactor = 1.0f;

        void ProcessInputEvent(const SDL_Event& event) {
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                activeKeys.insert(event.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                activeKeys.erase(event.key.key);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseDelta.x += event.motion.xrel;
                mouseDelta.y += event.motion.yrel;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                Engine::Get().EmitEvent(std::make_shared<CameraZoomEvent>(event.wheel.y));
                break;
            case SDL_EVENT_QUIT:
                exit(1);
                break;
            default:
                break;
            }
        }

        void ProcessContinuousMovement(float deltaTime) {
            glm::vec3 direction(0.0f);
            float moveSpeed = 50.0f * deltaTime; 

            if (activeKeys.contains(SDLK_W)) direction.z -= moveSpeed;
            if (activeKeys.contains(SDLK_S)) direction.z += moveSpeed;
            if (activeKeys.contains(SDLK_A)) direction.x -= moveSpeed;
            if (activeKeys.contains(SDLK_D)) direction.x += moveSpeed;
            if (activeKeys.contains(SDLK_Q)) direction.y += moveSpeed;
            if (activeKeys.contains(SDLK_E)) direction.y -= moveSpeed;

            if (glm::length(direction) > 0.0f) {
                Engine::Get().EmitEvent(std::make_shared<CameraMoveEvent>(glm::normalize(direction) * moveSpeed));
            }
        }

        void ProcessMouseMovement() {
            if (glm::length(mouseDelta) > 0.0f) {
                glm::vec2 smoothedDelta = glm::mix(prevMouseDelta, mouseDelta, mouseSmoothFactor);
                Engine::Get().EmitEvent(std::make_shared<CameraRotateEvent>(smoothedDelta.x, smoothedDelta.y));

                prevMouseDelta = smoothedDelta;
                mouseDelta = glm::vec2(0.0f); 
            }
        }
    };
}
