#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 color;          // Base color (green)
uniform vec3 lightDir;       // Directional light direction
uniform vec3 lightColor;     // Light color (e.g., white)
uniform float ambientStrength; // Ambient light intensity

void main()
{
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir); // Negative because light points toward surface
    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * lightColor;

    // Combine ambient and diffuse with base color
    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}