#version 330 core
// Description: A flat shader with uniform material color

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform float time;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main()
{
    vec3 ambient = material.ambient;
    vec3 color = vec3(0.0);
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        color += ambient * lights[i].color * lights[i].intensity;
    }
    FragColor = vec4(color, 1.0);
}
