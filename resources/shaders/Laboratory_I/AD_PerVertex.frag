#version 330 core
// Description: Simple pass-through fragment shader for AD_PerVertex

// Input from vertex shader
in vec4 vColor;
in vec2 TexCoords;

// Output
out vec4 FragColor;

// Light properties
uniform vec3 lightColor;
uniform float lightIntensity;

void main()
{
    // Simply pass through the interpolated vertex color, adjusted by light color/intensity
    FragColor = vColor * vec4(lightColor * lightIntensity, 1.0);
}
