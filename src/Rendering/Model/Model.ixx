export module Model;
import std;
import <GL/glew.h>;
import <glm/glm.hpp>;

export struct Model {
	GLuint VAO, VBO, EBO;
	unsigned int vertex_count;
	unsigned int index_count;
    glm::vec3 diffuseColor;

    static Model Create(const std::vector<float>& vertices, const std::vector<unsigned int>& indices = {}, const glm::vec3& diffuseColor = glm::vec3(1.0f))
    {
        Model model{ 0, 0, 0, 0, 0, diffuseColor };

        glGenVertexArrays(1, &model.VAO);
        glBindVertexArray(model.VAO);

        glGenBuffers(1, &model.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Assuming 5 floats per vertex (x, y, z, u, v) from ModelManager
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // UV
        glEnableVertexAttribArray(1);

        if (!indices.empty())
        {
            glGenBuffers(1, &model.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            model.index_count = static_cast<unsigned int>(indices.size());
        }

        // Always set vertex_count based on vertices input
        model.vertex_count = static_cast<unsigned int>(vertices.size() / 5); // 5 floats per vertex

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return model;
    }

	void Destroy() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		if (index_count > 0) {
			glDeleteBuffers(1, &EBO);
		}
		VAO = VBO = EBO = 0;
		vertex_count = index_count = 0;
	}
};