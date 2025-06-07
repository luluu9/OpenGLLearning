#version 330 core
// Description: Vertex shader for wave distortion effect

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    float WaveDisplacement;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
const float WAVE_AMPLITUDE = 0.25;
const float WAVE_SPEED = 1.0;

void main()
{
    vec4 modelPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = vec3(modelPos);
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoord = aTexCoord;
    float displacement = sin(modelPos.x + time * WAVE_SPEED) * 
                        cos(modelPos.z + time * WAVE_SPEED) * 
                        WAVE_AMPLITUDE;
    vs_out.WaveDisplacement = displacement;
    modelPos.y += displacement;
    gl_Position = projection * view * modelPos;
}
