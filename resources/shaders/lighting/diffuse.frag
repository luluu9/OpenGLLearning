#version 330 core
// Description: A diffuse lighting shader that considers light direction

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform float time;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * material.ambient;
    vec3 norm = normalize(Normal);
    vec3 diffuse = vec3(0.0);
    
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        diffuse += diff * material.diffuse * lights[i].color * lights[i].intensity * attenuation;
    }
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
