export module RenderingSystem;
import SystemManager;
import Engine;
import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <GL/glew.h>;
import <SDL3/SDL.h>; 
import RenderableComponent;
import Model;
import MeshComponent;
import TransformComponent;
import CameraEvents;

namespace ECS {
    export class RenderingSystem : public System
    {
    public:
        void Init()
        {
            Signature signature;
            signature.set(Engine::Get().GetComponentType<Renderable>());
            Engine::Get().SetSystemSignature<RenderingSystem>(signature);

            Engine::Get().Subscribe(CAMERA_MOVE_EVENT, [this](const Event& event) {
                const auto& moveEvent = static_cast<const CameraMoveEvent&>(event);
                HandleCameraMove(moveEvent.GetDirection());
                ComputeMatrixes();
                });

            Engine::Get().Subscribe(CAMERA_ROTATE_EVENT, [this](const Event& event) {
                const auto& rotateEvent = static_cast<const CameraRotateEvent&>(event);
                HandleCameraRotate(rotateEvent.GetDeltaX(), rotateEvent.GetDeltaY());
                ComputeMatrixes();
                });

            Engine::Get().Subscribe(CAMERA_ZOOM_EVENT, [this](const Event& event) {
                const auto& zoomEvent = static_cast<const CameraZoomEvent&>(event);
                HandleCameraZoom(zoomEvent.GetDeltaZoom());
                ComputeMatrixes();
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
                auto& transform = Engine::Get().GetComponent<Transform>(entity);
                batches[renderable.shader_program][mesh.model].push_back(GeometryWrapper{ &renderable, &transform });

            }

            for (const auto& [shader, model_map] : batches)
            {
                glUseProgram(shader);
                glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

                for (const auto& [model, renderables] : model_map) {
                    if (renderables.size() > 1) {
                        RenderInstanced(model, renderables);
                    }
                    else {
                        RenderSingle(model, renderables[0].renderable, renderables[0].transform);
                    }
                }
            }

        }

    private:
        struct GeometryWrapper {
            Renderable* renderable;
            Transform* transform;
        };
        float dt;

        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 camera_position = glm::vec3(0,10,5);
        glm::vec3 camera_target = glm::vec3(0,10,0);
        glm::vec3 camera_up = glm::vec3(0,1,0);
        float FOV = 45.0f;
        float aspect_ratio = 1280.0f / 768.0f;
        float near_plane = 0.1f;
        float far_plane = 100.0f;

        void ComputeMatrixes()
        {
            view = glm::lookAt(camera_position, camera_target, camera_up);
            projection = glm::perspective(glm::radians(FOV), aspect_ratio, near_plane, far_plane);
        }

        void RenderSingle(const std::shared_ptr<Model>& model, const Renderable* renderable, const Transform* transform) {
            glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(transform->position)) *
                glm::scale(glm::mat4(1), transform->scale);

            glUniformMatrix4fv(glGetUniformLocation(renderable->shader_program, "model"), 1, GL_FALSE, &matrix[0][0]);
            glUniform3fv(glGetUniformLocation(renderable->shader_program, "color"), 1, &renderable->color[0]);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderable->texture_ID);
            glUniform1i(glGetUniformLocation(renderable->shader_program, "texture1"), 0);

            glBindVertexArray(model->VAO);
            if (model->index_count > 0) {
                glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);
            }
            else {
                glDrawArrays(GL_TRIANGLES, 0, model->vertex_count);
            }
            glBindVertexArray(0);
        }

        void RenderInstanced(const std::shared_ptr<Model>& model, const std::vector<GeometryWrapper>& renderables) {
            GLuint instance_VBO;
            glGenBuffers(1, &instance_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
            std::vector<glm::mat4> transforms;

            for (const auto& r : renderables) {
                glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(r.transform->position)) *
                    glm::scale(glm::mat4(1), r.transform->scale);
                transforms.push_back(matrix);
            }

            glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_STATIC_DRAW);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderables[0].renderable->texture_ID);
            glUniform1i(glGetUniformLocation(renderables[0].renderable->shader_program, "texture1"), 0);
            glUniform3fv(glGetUniformLocation(renderables[0].renderable->shader_program, "color"), 1,
                &renderables[0].renderable->color[0]);

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

        void SetupInstancedAttributes(GLuint instance_VBO) {
            for (int i = 0; i < 4; i++) {
                glEnableVertexAttribArray(3 + i);
                glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                    (void*)(i * sizeof(glm::vec4)));
                glVertexAttribDivisor(3 + i, 1);
            }
        }


        void HandleCameraMove(const glm::vec3& direction)
        {
            const float cameraSpeed = 5.0f;
            // Calculate camera basis vectors
            glm::vec3 forward = glm::normalize(camera_target - camera_position);
            glm::vec3 right = glm::normalize(glm::cross(forward, camera_up));
            glm::vec3 up = camera_up; // Usually (0,1,0)

            // Move along camera-relative axes
            glm::vec3 moveDirection =
                forward * (-direction.z) + // Forward/backward (Z) - negative because forward is -Z in OpenGL
                right * direction.x +      // Left/right (X)
                up * direction.y;          // Up/down (Y)

            // Apply movement
            camera_position += moveDirection * cameraSpeed * dt;
            camera_target += moveDirection * cameraSpeed * dt;
        }

        void HandleCameraRotate(float deltaX, float deltaY)
        {
            const float sensitivity = 0.1f;

            glm::vec3 direction = glm::normalize(camera_target - camera_position);
            glm::vec3 right = glm::normalize(glm::cross(direction, camera_up));


            glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaX * sensitivity), camera_up);
            direction = glm::vec3(yawRotation * glm::vec4(direction, 0.0f));

            right = glm::normalize(glm::cross(direction, camera_up));
            glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaY * sensitivity), right);
            direction = glm::vec3(pitchRotation * glm::vec4(direction, 0.0f));


            camera_target = camera_position + direction * dt;
        }

        void HandleCameraZoom(float deltaZoom)
        {
            const float zoomSpeed = 2.0f; 
            FOV -= deltaZoom * zoomSpeed;
            FOV = glm::clamp(FOV, 1.0f, 90.0f); 
        }


    };
}