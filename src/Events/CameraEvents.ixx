// CameraEvents.h
export module CameraEvents;
import EventManager;
import <glm/glm.hpp>;
import std;
import Globals;

export class CameraMoveEvent : public ECS::Event {
public:
    CameraMoveEvent(const glm::vec3& direction) : direction(direction) {}

    EventType GetType() const override {
        return CAMERA_MOVE_EVENT;
    }

    glm::vec3 GetDirection() const {
        return direction;
    }

private:
    glm::vec3 direction; // Direction of movement
};

export class CameraRotateEvent : public ECS::Event {
public:
    CameraRotateEvent(float deltaX, float deltaY) : deltaX(deltaX), deltaY(deltaY) {}

    EventType GetType() const override {
        return CAMERA_ROTATE_EVENT;
    }

    float GetDeltaX() const {
        return deltaX;
    }

    float GetDeltaY() const {
        return deltaY;
    }

private:
    float deltaX; // Horizontal mouse movement
    float deltaY; // Vertical mouse movement
};

export class CameraZoomEvent : public ECS::Event {
public:
    CameraZoomEvent(float deltaZoom) : deltaZoom(deltaZoom) {}

    EventType GetType() const override {
        return CAMERA_ZOOM_EVENT;
    }

    float GetDeltaZoom() const {
        return deltaZoom;
    }

private:
    float deltaZoom; // Scroll wheel delta
};