module;
#include <glm/glm.hpp>
export module CameraComponent;

export struct Camera {
    glm::vec3 position;     
    glm::mat4 view;         
    glm::mat4 projection;   
};