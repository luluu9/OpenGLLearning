#version 330 core
// Description: Simple pass-through fragment shader for ADS_PerVertex_Gouraud

// Input from vertex shader
in vec4 vColor;
in vec2 TexCoords;

// Output
out vec4 FragColor;

void main()
{
    // Simply pass through the interpolated vertex color (lighting already calculated in vertex shader)
    FragColor = vColor;
}
