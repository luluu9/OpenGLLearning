#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    vPosition = aPos;
    vNormal = aNormal;
    vTexCoord = aTexCoord;
}
