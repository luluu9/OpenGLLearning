#version 330 core
// Description: Fragment shader for Phong shading (all lighting calculated per fragment)

in vec3 FragPos;
in vec3 Normal;
in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoords;

out vec4 FragColor;

uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

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
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(ViewDir);
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * material.ambient;
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        diffuseColor += diffuseFactor * material.diffuse * lights[i].color * lights[i].intensity * attenuation;

        if (diffuseFactor > 0.0) {
            vec3 reflectionVector = reflect(-lightDir, normal);
            float specularFactor = pow(max(dot(viewDir, reflectionVector), 0.0), material.shininess);
            specularColor += specularFactor * material.specular * lights[i].color * lights[i].intensity * attenuation;
        }
    }

    vec3 result = ambientColor + diffuseColor + specularColor;
    FragColor = vec4(result, 1.0);
}
