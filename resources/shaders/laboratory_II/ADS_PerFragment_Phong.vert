#version 330 core
// Description: Vertex shader for Phong shading (all lighting calculated in fragment shader)

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform float time;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    LightDir = (numLights > 0) ? lights[0].position - FragPos : vec3(0.0, 1.0, 0.0);
    ViewDir = viewPos - FragPos;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
