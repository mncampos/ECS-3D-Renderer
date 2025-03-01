export module ShaderManager;
import std;
import <GL/glew.h>;

export class ShaderManager
{
public:

	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	static ShaderManager& Get()
	{
		static ShaderManager instance;
		return instance;
	}

	GLuint GetProgram(const std::string& vertex_shader_path, const  std::string& fragment_shader_path)
	{
		std::string key = vertex_shader_path + "|" + fragment_shader_path;
		if (shaders.find(key) != shaders.end())
		{
			return shaders[key];
		}
		else {
			GLuint program = LoadShaderFromFiles(vertex_shader_path, fragment_shader_path);
			if (program != 0)
			{
				shaders[key] = program;
			}
			else {
				std::cerr << "[ShaderManager::GetProgram] ERROR  -> Failed to load shader " << vertex_shader_path << " & " << fragment_shader_path << std::endl;
			}
			return program;
		}
	}

	void Cleanup()
	{
		for (auto& [key, shader] : shaders) {
			glDeleteProgram(shader);
		}
		shaders.clear();
	}

private:
	ShaderManager() = default;
	std::unordered_map<std::string, GLuint> shaders;

    GLuint LoadShaderFromFiles(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {

        std::string vertex_code = readFile(vertex_shader_path);
        std::string fragment_code = readFile(fragment_shader_path);
        if (vertex_code.empty() || fragment_code.empty()) {
            return 0;
        }

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertex_code.c_str());
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragment_code.c_str());
        if (vertexShader == 0 || fragmentShader == 0) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return 0;
        }

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);


        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "[ShaderManager::LoadShaderFromFiles] ERROR : " << infoLog << std::endl;
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "[ShaderManager::LoadShaderFromFiles] ERROR : Failed to open file: " << path << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                << " shader compilation failed: " << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

};