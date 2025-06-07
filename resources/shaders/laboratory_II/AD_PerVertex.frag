#version 330 core
// Description: Simple pass-through fragment shader for AD_PerVertex

in vec4 vColor;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
