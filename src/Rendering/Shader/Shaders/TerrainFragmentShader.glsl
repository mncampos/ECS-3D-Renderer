#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 color;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir);
    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0);
}