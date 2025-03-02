#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;

uniform vec3 lightDir;        // Normalized light direction
uniform vec3 lightColor;      // Light color (e.g., white)
uniform vec3 viewPos;         // Camera position for specular

struct Material {
    vec3 diffuse;
};
uniform Material material;

void main()
{
    // Use material's diffuse color
    vec3 baseColor = material.diffuse;
    
    // Fallback normal (upward, since manModel lacks normals)
    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 light = normalize(-lightDir);
    float diff = max(dot(normal, light), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (shiny reflection for silver)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess of 32
    vec3 specular = 0.5 * spec * lightColor; // Specular strength
    
    // Combine
    vec3 result = (ambient + diffuse + specular) * baseColor;
    FragColor = vec4(result, 1.0);
}