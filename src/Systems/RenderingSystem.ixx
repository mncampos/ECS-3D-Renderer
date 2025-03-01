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
            signature.set(Engine::Get().GetComponentType<Mesh>()); // StaticMesh is Mesh in your setup
            signature.set(Engine::Get().GetComponentType<Position>());
            Engine::Get().SetSystemSignature<RenderingSystem>(signature);

            // Subscribe to camera updates from CameraSystem
            Engine::Get().Subscribe(CAMERA_UPDATE_EVENT, [this](const Event& event) {

                const auto& cameraEvent = static_cast<const CameraUpdateEvent&>(event);
                view = cameraEvent.view;
                projection = cameraEvent.projection;
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

                for (const auto& [model, renderables] : model_map)
                {
                    if (renderables.size() > 1) {
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

        void RenderSingle(const std::shared_ptr<Model>& model, const Renderable* renderable, const Position* position)
        {
            // Translate using Position (x, z, y), no scale since Position lacks it
            glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position->x, position->z, position->y));

            glUniformMatrix4fv(glGetUniformLocation(renderable->shader_program, "model"), 1, GL_FALSE, &matrix[0][0]);
            glUniform3fv(glGetUniformLocation(renderable->shader_program, "color"), 1, &renderable->color[0]);

            // No texture binding (solid color for man.obj)
            glBindVertexArray(model->VAO);
            if (model->index_count > 0) {
                glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(GL_TRIANGLES, 0, model->vertex_count);
            }
            glBindVertexArray(0);
        }

        void RenderInstanced(const std::shared_ptr<Model>& model, const std::vector<GeometryWrapper>& renderables)
        {
            GLuint instance_VBO;
            glGenBuffers(1, &instance_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
            std::vector<glm::mat4> transforms;

            for (const auto& r : renderables) {
                glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(r.position->x, r.position->z, r.position->y));
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