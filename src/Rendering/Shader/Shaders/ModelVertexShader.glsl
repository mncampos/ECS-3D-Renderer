#version 330 core
layout(location = 0) in vec3 aPos;       // Position (x, y, z)
layout(location = 1) in vec2 aTexCoord;  // UVs (u, v)
layout(location = 2) in vec3 aColor;     // Vertex color (r, g, b), if present
layout(location = 3) in mat4 aModel;     // Instanced model matrix (locations 3, 4, 5, 6)

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Color;

void main()
{
    vec4 pos = aModel * vec4(aPos, 1.0);
    gl_Position = projection * view * pos;
    FragPos = vec3(pos); // World-space position for lighting
    Color = aColor;      // Pass vertex color (if available)
}