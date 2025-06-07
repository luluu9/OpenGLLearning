#version 330 core
// Description: Per-vertex ambient and diffuse lighting using Lambert model

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 vColor;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

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
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    mat4 viewModel = view * model;
    vec4 eyePosition = viewModel * vec4(aPos, 1.0);
    vec3 eyePositionVec3 = eyePosition.xyz;
    vec3 eyeNormal = normalize(mat3(transpose(inverse(viewModel))) * aNormal);
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * material.ambient;
    vec3 diffuseColor = vec3(0.0);

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 eyeLightPos = vec3(view * vec4(lights[i].position, 1.0));
        vec3 lightVector = normalize(eyeLightPos - eyePositionVec3);
        float diffuseFactor = max(dot(eyeNormal, lightVector), 0.0);
        float distance = length(eyeLightPos - eyePositionVec3);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        diffuseColor += diffuseFactor * material.diffuse * lights[i].color * lights[i].intensity * attenuation;
    }

    vColor = vec4(ambientColor + diffuseColor, 1.0);
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}
