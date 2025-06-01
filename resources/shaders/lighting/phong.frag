#version 330 core
// Description: A full Phong lighting model with ambient, diffuse and specular components

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
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * material.specular;
    
    vec3 result = (ambient + diffuse + specular) * lightColor * lightIntensity;
    FragColor = vec4(result, 1.0);
}
