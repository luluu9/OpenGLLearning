#version 330 core
// Description: Gouraud shading with ambient, diffuse, and specular components (per-vertex)

// Inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Outputs
out vec4 vColor;  // Output color from vertex shader to fragment shader
out vec2 TexCoords;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;  // Light position in world space
uniform vec3 viewPos;   // View/camera position in world space

// Material properties
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

// Light properties
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform float time; // Global time for animations

void main()
{
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    mat4 viewModel = view * model;
    vec4 eyePosition = viewModel * vec4(aPos, 1.0);
    vec3 eyePositionVec3 = eyePosition.xyz;
    vec3 eyeNormal = normalize(mat3(transpose(inverse(viewModel))) * aNormal);
    vec3 ambientColor = vec3(0.1) * material.ambient;
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 eyeLightPos = vec3(view * vec4(lights[i].position, 1.0));
        vec3 lightVector = normalize(eyeLightPos - eyePositionVec3);
        float diffuseFactor = max(dot(eyeNormal, lightVector), 0.0);
        float distance = length(eyeLightPos - eyePositionVec3);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        diffuseColor += diffuseFactor * material.diffuse * lights[i].color * attenuation * lights[i].intensity;

        if (diffuseFactor > 0.0) {
            vec3 viewVector = normalize(-eyePositionVec3);
            vec3 reflectionVector = reflect(-lightVector, eyeNormal);
            float specularFactor = pow(max(dot(reflectionVector, viewVector), 0.0), material.shininess);
            specularColor += specularFactor * material.specular * lights[i].color * attenuation * lights[i].intensity;
        }
    }

    vColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}
