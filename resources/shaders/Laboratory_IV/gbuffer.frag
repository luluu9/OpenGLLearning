#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main()
{
    // Store position in world space
    gPosition = FragPos;
    
    // Store the normalized normal
    gNormal = normalize(Normal);
    
    // Store the diffuse color in RGB and specular intensity in A
    gAlbedoSpec.rgb = material.diffuse;
    gAlbedoSpec.a = material.shininess / 256.0; // Normalize shininess to 0-1 range
}
