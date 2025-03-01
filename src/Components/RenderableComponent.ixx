export module RenderableComponent;
import  <glm/glm.hpp>;
import  <GL/glew.h>;

export struct Renderable {
    GLuint shader_program; 
    GLuint texture_ID;    
    glm::vec3 color;
};