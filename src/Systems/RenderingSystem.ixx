module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL3/SDL.h>
export module RenderingSystem;
import SystemManager;
import Engine;
import RenderableComponent;
import Model;
import MeshComponent;
import PositionComponent; 
import CameraEvents;

namespace ECS {
    export class RenderingSystem : public System
    {
    public:
        void Init()
        {
            Signature signature;
            signature.set(Engine::Get().GetComponentType<Renderable>());
            signature.set(Engine::Get().GetComponentType<Mesh>()); 
            signature.set(Engine::Get().GetComponentType<Position>());
            Engine::Get().SetSystemSignature<RenderingSystem>(signature);


            Engine::Get().Subscribe(CAMERA_UPDATE_EVENT, [this](const Event& event) {

                const auto& cameraEvent = static_cast<const CameraUpdateEvent&>(event);
                view = cameraEvent.view;
                projection = cameraEvent.projection;
                position = cameraEvent.position;
                });
        }

        void Update(float delta_time) override
        {
            dt = delta_time;

            std::unordered_map<GLuint, std::unordered_map<std::shared_ptr<Model>, std::vector<GeometryWrapper>>> batches;

            for (const auto& entity : Entities)
            {
                auto& mesh = Engine::Get().GetComponent<Mesh>(entity);
                auto& renderable = Engine::Get().GetComponent<Renderable>(entity);
                auto& position = Engine::Get().GetComponent<Position>(entity);
                batches[renderable.shader_program][mesh.model].push_back(GeometryWrapper{ &renderable, &position });
            }

            for (const auto& [shader, model_map] : batches)
            {
                glUseProgram(shader);
                glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

                glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
                glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
                glUniform3fv(glGetUniformLocation(shader, "lightDir"), 1, &lightDir[0]);
                glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, &lightColor[0]);
                glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &position[0]);

                for (const auto& [model, renderables] : model_map)
                {
                    if (renderables.size() > 1) {
                        std::cout << "a";
                        RenderInstanced(model, renderables);
                    }
                    else {
                        RenderSingle(model, renderables[0].renderable, renderables[0].position);
                    }
                }
            }
        }

    private:
        struct GeometryWrapper {
            Renderable* renderable;
            Position* position; // Changed from Transform
        };
        float dt;

        glm::mat4 view = glm::mat4(1.0f);      // Default until camera updates
        glm::mat4 projection = glm::mat4(1.0f); // Default until camera updates
        glm::vec3 position = glm::vec3(0);

        void RenderSingle(const std::shared_ptr<Model>& model, const Renderable* renderable, const Position* position)
        {
            // Build the transformation matrix
            glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position->x, position->z, position->y));
            glm::vec3 facingDirection = glm::vec3(position->facing.x, 0.0f, position->facing.y);
            if (glm::length(facingDirection) > 0.001f) { // Small epsilon to avoid NaN
                facingDirection = glm::normalize(facingDirection);
                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
                glm::mat4 rotationMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0f), facingDirection, up));
                matrix = matrix * rotationMatrix;
            }

            // Setup instance VBO for single entity
            GLuint instance_VBO;
            glGenBuffers(1, &instance_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &matrix[0][0], GL_STATIC_DRAW);

            // Set color uniform
            glUniform3fv(glGetUniformLocation(renderable->shader_program, "color"), 1, &renderable->color[0]);

            glUniform3fv(glGetUniformLocation(renderable->shader_program, "material.diffuse"), 1, &model->diffuseColor[0]);

            // Bind VAO and setup instanced attributes
            glBindVertexArray(model->VAO);
            SetupInstancedAttributes(instance_VBO); // Reuse the same setup as instancing
            if (model->index_count > 0) {
                glDrawElementsInstanced(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0, 1); // 1 instance
            }
            else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, model->vertex_count, 1); // 1 instance
            }

            // Cleanup
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &instance_VBO);
        }

        void RenderInstanced(const std::shared_ptr<Model>& model, const std::vector<GeometryWrapper>& renderables)
        {
            GLuint instance_VBO;
            glGenBuffers(1, &instance_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
            std::vector<glm::mat4> transforms;

            for (const auto& r : renderables) {
                glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(r.position->x, r.position->z, r.position->y));

                // Calculate the rotation matrix based on the facing vector
                glm::vec3 facingDirection = glm::vec3(r.position->facing.x, 0.0f, r.position->facing.y);
                if (glm::length(facingDirection) > 0.0f)
                {
                    facingDirection = glm::normalize(facingDirection);
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Assuming Y is up
                    glm::mat4 rotationMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0f), facingDirection, up));
                    matrix = matrix * rotationMatrix;
                }

                transforms.push_back(matrix);
            }

            glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_STATIC_DRAW);

            // No texture binding
            glUniform3fv(glGetUniformLocation(renderables[0].renderable->shader_program, "color"), 1, &renderables[0].renderable->color[0]);

            glBindVertexArray(model->VAO);
            SetupInstancedAttributes(instance_VBO);
            if (model->index_count > 0) {
                glDrawElementsInstanced(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0, (GLsizei)renderables.size());
            }
            else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, model->vertex_count, (GLsizei)renderables.size());
            }
            glBindVertexArray(0);
            glDeleteBuffers(1, &instance_VBO);
        }

        void SetupInstancedAttributes(GLuint instance_VBO)
        {
            for (int i = 0; i < 4; i++) {
                glEnableVertexAttribArray(3 + i);
                glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
                glVertexAttribDivisor(3 + i, 1);
            }
        }
    };
}