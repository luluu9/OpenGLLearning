#version 330 core
// Description: A flat shader with uniform material color

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// Light properties
uniform vec3 lightColor;
uniform float lightIntensity;

void main()
{    vec3 ambient = material.ambient;
    
    FragColor = vec4(ambient * lightColor * lightIntensity, 1.0);
}
