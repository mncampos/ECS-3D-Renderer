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

			Engine::Get().Subscribe(CAMERA_UPDATE_EVENT, [this](const Event& event) {
				const auto& cameraEvent = static_cast<const CameraUpdateEvent&>(event);
				view = cameraEvent.view;
				projection = cameraEvent.projection;
				});

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
		glm::mat4 view = glm::mat4(1.0f);      
		glm::mat4 projection = glm::mat4(1.0f);
	};
}