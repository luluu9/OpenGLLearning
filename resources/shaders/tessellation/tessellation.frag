#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Light properties
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float lightIntensity;

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
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * material.ambient;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;
    
    // Attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Result
    vec3 result = (ambient + diffuse + specular) * lightColor * lightIntensity;
    FragColor = vec4(result, 1.0);
}
