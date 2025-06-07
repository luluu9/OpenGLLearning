#version 330 core
// Description: Vertex shader for chrome/reflective material effect

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 ReflectDir;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Camera position for reflection vector
uniform vec3 viewPos;

void main()
{
    // Calculate world-space position and normal
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = vec3(worldPos);
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    
    // Calculate the reflection vector for environment mapping
    vec3 viewDir = normalize(viewPos - vs_out.FragPos);
    vs_out.ReflectDir = reflect(-viewDir, normalize(vs_out.Normal));
    
    // Calculate clip-space position
    gl_Position = projection * view * worldPos;
}
