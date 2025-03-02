// CameraEvents.h
export module CameraEvents;
import EventManager;
import <glm/glm.hpp>;
import std;
import Globals;

export class CameraUpdateEvent : public ECS::Event {
public:
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;


    CameraUpdateEvent(const glm::mat4& v, const glm::mat4& p, const glm::vec3 pos) : view(v), projection(p), position(pos) {}

    EventType GetType() const override {
        return CAMERA_UPDATE_EVENT;
    }
};
