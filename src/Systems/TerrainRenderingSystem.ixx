module;
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
export module TerrainRenderingSystem;
import Engine;
import SystemManager;
import RenderableComponent;
import TerrainMeshComponent;
import TransformComponent;
import CameraEvents;

namespace ECS {
	export class TerrainRenderingSystem : public System
	{
	public:
		void Init()
		{
			Signature signature;
			signature.set(Engine::Get().GetComponentType<Renderable>());
			signature.set(Engine::Get().GetComponentType<TerrainMesh>());
			signature.set(Engine::Get().GetComponentType<Transform>());
			Engine::Get().SetSystemSignature<TerrainRenderingSystem>(signature);

			Engine::Get().Subscribe(CAMERA_MOVE_EVENT, [this](const Event& event) {
				const auto& moveEvent = static_cast<const CameraMoveEvent&>(event);
				HandleCameraMove(moveEvent.GetDirection());
				ComputeMatrices();
				});

			Engine::Get().Subscribe(CAMERA_ROTATE_EVENT, [this](const Event& event) {
				const auto& rotateEvent = static_cast<const CameraRotateEvent&>(event);
				HandleCameraRotate(rotateEvent.GetDeltaX(), rotateEvent.GetDeltaY());
				ComputeMatrices();
				});

			Engine::Get().Subscribe(CAMERA_ZOOM_EVENT, [this](const Event& event) {
				const auto& zoomEvent = static_cast<const CameraZoomEvent&>(event);
				HandleCameraZoom(zoomEvent.GetDeltaZoom());
				ComputeMatrices();
				});

			// Initial matrix computation
			ComputeMatrices();
		}

		void Update(float delta_time) override
		{
			for (const auto& entity : Entities)
			{
				auto& renderable = Engine::Get().GetComponent<Renderable>(entity);
				auto& mesh = Engine::Get().GetComponent<TerrainMesh>(entity);
				auto& transform = Engine::Get().GetComponent<Transform>(entity);

				glUseProgram(renderable.shader_program);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position)) *
					glm::scale(glm::mat4(1.0f), transform.scale);
				glUniformMatrix4fv(glGetUniformLocation(renderable.shader_program, "model"), 1, GL_FALSE, &model[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(renderable.shader_program, "view"), 1, GL_FALSE, &view[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(renderable.shader_program, "projection"), 1, GL_FALSE, &projection[0][0]);
				glUniform3fv(glGetUniformLocation(renderable.shader_program, "color"), 1, &renderable.color[0]);

				            glm::vec3 lightDir = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
            glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); 
            float ambientStrength = 0.3f; 
            glUniform3fv(glGetUniformLocation(renderable.shader_program, "lightDir"), 1, &lightDir[0]);
            glUniform3fv(glGetUniformLocation(renderable.shader_program, "lightColor"), 1, &lightColor[0]);
            glUniform1f(glGetUniformLocation(renderable.shader_program, "ambientStrength"), ambientStrength);


				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}


	private:
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 camera_position = glm::vec3(0, 10, 5);  // Default above the grid
		glm::vec3 camera_target = glm::vec3(0, 0, 0);     // Looking at origin
		glm::vec3 camera_up = glm::vec3(0, 1, 0);         // Up direction
		float FOV = 45.0f;
		float aspect_ratio = 1280.0f / 768.0f;           // Match your window
		float near_plane = 0.1f;
		float far_plane = 100.0f;

		void ComputeMatrices()
		{
			view = glm::lookAt(camera_position, camera_target, camera_up);
			projection = glm::perspective(glm::radians(FOV), aspect_ratio, near_plane, far_plane);
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
			camera_position += moveDirection * cameraSpeed ;
			camera_target += moveDirection * cameraSpeed;
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


			camera_target = camera_position + direction ;
		}

		void HandleCameraZoom(float deltaZoom)
		{
			const float zoomSpeed = 2.0f;
			FOV -= deltaZoom * zoomSpeed;
			FOV = glm::clamp(FOV, 1.0f, 90.0f);
		}

	};
}