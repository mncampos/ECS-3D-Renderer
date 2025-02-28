export module RenderingSystem;
import SystemManager;
import Engine;
import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <GL/glew.h>;
import GeometryComponents;
import GraphicsComponents;
import <SDL3/SDL.h>; // Add this for SDL3 image loading

namespace ECS {
    export class RenderingSystem : public System
    {
    private:
        GLuint vao;  // Vertex Array Object
        GLuint vbo;  // Vertex Buffer Object
        GLuint ebo;  // Element Buffer Object
        GLuint fboID; // Face info buffer object
        GLuint grassTexture;  // Grass texture ID
        GLuint dirtTexture;   // Wall texture ID
        float rotationAngle = 0.0f;

        // Vertex structure to hold position and texture coordinates
        struct Vertex {
            glm::vec3 position;
            glm::vec2 texCoord;
        };

        // Helper function to load texture using SDL3
        GLuint LoadTexture(const char* path) {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Set texture wrapping/filtering options

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Load BMP using SDL3
            SDL_Surface* surface = SDL_LoadBMP(path);
            if (surface == nullptr) {
                std::cout << "Failed to load texture: " << path << std::endl;
                std::cout << "SDL Error: " << SDL_GetError() << std::endl;
                return 0;
            }

            // Determine format based on SDL surface format
            SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
            if (convertedSurface == nullptr) {
                std::cout << "Failed to convert surface format: " << SDL_GetError() << std::endl;
                SDL_DestroySurface(surface);
                return 0;
            }

            // Upload texture data to OpenGL using the converted surface
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertedSurface->w, convertedSurface->h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Free the SDL surfaces
            SDL_DestroySurface(convertedSurface);
            SDL_DestroySurface(surface);

            std::cout << "Loaded texture: " << path << " with ID: " << textureID << std::endl;
            return textureID;
        }

    public:
        void Init()
        {
            // Load textures
            grassTexture = LoadTexture("assets/Textures/grass.bmp");
            dirtTexture = LoadTexture("assets/Textures/dirt.bmp");

            Entity test = engine.createEntity();
            engine.AddComponent(test, Transform{ glm::ivec3(0, 0, 0) });

            Mesh mesh;

            // Create vertices with positions and texture coordinates for each face separately
            float vertices[] = {
                // Back face (Z = -0.5)
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f, // faceID = 1 (dirt)
                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f,

                // Front face (Z = 0.5)
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, // faceID = 1 (dirt)
                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,

                // Left face (X = -0.5)
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, // faceID = 1 (dirt)
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,

                // Right face (X = 0.5)
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, // faceID = 1 (dirt)
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
                 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,

                 // Bottom face (Y = -0.5)
                 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, // faceID = 1 (dirt)
                  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
                  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,
                  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,
                 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
                 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,

                 // Top face (Y = 0.5) - Use grass texture
                 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, // faceID = 0 (grass)
                  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,
                  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
                  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
                 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,
                 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f
            };


            mesh.texture_id = grassTexture;
            mesh.texture_id2 = dirtTexture;
            engine.AddComponent(test, mesh);

            // Initialize OpenGL buffers
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            // Position and texture coordinates buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Texture coordinates attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);

        }

        void Update(float delta_time) override
        {
            // Update rotation angle
            rotationAngle += 45.0f * delta_time; // 45 degrees per second
            if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;

            // Create and compile shaders
            GLuint shaderProgram = CreateShaderProgram();

            // Set up matrices
            glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1));

            glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 5.0f); // Camera at an isometric angle
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);    // Look at the cube's center
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);       // Up is positive Y
            glm::mat4 view = glm::lookAt(cameraPos, target, up);

            glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                1280.0f / 768.0f, // aspect ratio
                0.1f, 100.0f);

            // Clear the frame
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Enable depth testing for proper 3D rendering
            glEnable(GL_DEPTH_TEST);

            // Use shader program and set uniforms
            glUseProgram(shaderProgram);

            // Set matrix uniforms
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                1, GL_FALSE, &projection[0][0]);

            // Set texture uniforms
            glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 0);
            glUniform1i(glGetUniformLocation(shaderProgram, "dirtTexture"), 1);

            // Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, grassTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, dirtTexture);

            // Draw the cube
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        GLuint CreateShaderProgram() {
            // Vertex shader with texture support
            const char* vertexShaderSource = R"(
                #version 460 core
                layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec2 aTexCoord;
                layout (location = 2) in float aFaceID;

                
                out vec2 TexCoord;
                out float FaceID;

                
                uniform mat4 model;
                uniform mat4 view;
                uniform mat4 projection;
                
                void main()
                {
                    gl_Position = projection * view * model * vec4(aPos, 1.0);
                    TexCoord = aTexCoord;
                    FaceID = aFaceID;
                }
            )";

            // Fragment shader for textured rendering
            const char* fragmentShaderSource = R"(
                #version 460 core
                out vec4 FragColor;
                
                in vec2 TexCoord;
                in float FaceID;
                
                uniform sampler2D grassTexture;
                uniform sampler2D dirtTexture;
                
                void main()
                {

                    if (FaceID == 0.0) {
            FragColor = texture(grassTexture, TexCoord); // Top face
        } else {
            FragColor = texture(dirtTexture, TexCoord);  // Other faces
        }
                }
            )";

            // Compile vertex shader
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);

            // Check for vertex shader compilation errors
            int success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

            // Compile fragment shader
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);

            // Check for fragment shader compilation errors
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

            // Create shader program
            GLuint shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            // Check for linking errors
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }

            // Free shader resources
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            return shaderProgram;
        }

          };
}