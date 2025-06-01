#version 330 core
// Description: A diffuse lighting shader that considers light direction

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
{    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * material.ambient;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;
    
    vec3 result = (ambient + diffuse) * lightColor * lightIntensity;
    FragColor = vec4(result, 1.0);
}
