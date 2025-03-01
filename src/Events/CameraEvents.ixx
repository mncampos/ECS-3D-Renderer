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


    CameraUpdateEvent(const glm::mat4& v, const glm::mat4& p) : view(v), projection(p) {}

    EventType GetType() const override {
        return CAMERA_UPDATE_EVENT;
    }
};
