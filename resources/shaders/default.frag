#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

// Camera position for specular calculation
uniform vec3 viewPos;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// Lighting model selection (0 = Flat, 1 = Phong)
uniform int lightingModel;

void main()
{
    // Normalize normal vector
    vec3 norm = normalize(Normal);
    
    // Calculate ambient component
    vec3 ambient = material.ambient * lightColor;
    
    // Variable to store the final color result
    vec3 result;
    
    if (lightingModel == 0) // Flat lighting
    {
        // Calculate light direction
        vec3 lightDir = normalize(lightPos - FragPos);
        
        // Simple diffuse calculation without interpolated normals
        float diff = max(dot(norm, lightDir), 0.0);
        
        // Simple distance attenuation
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // For flat shading, we use a simpler formula without specular
        vec3 diffuse = diff * material.diffuse * lightColor;
        
        // Combine lighting with flat appearance (no specular)
        result = (ambient + diffuse) * attenuation * lightIntensity;
    }
    else if (lightingModel == 1) // Blinn-Phong lighting
    {
        // Calculate light direction and distance
        vec3 lightDir = normalize(lightPos - FragPos);
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Calculate diffuse component
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * material.diffuse * lightColor;
        
        // Calculate specular component (Blinn-Phong)
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        vec3 specular = spec * material.specular * lightColor;
        
        // Combine all lighting components with attenuation and intensity
        result = (ambient + diffuse + specular) * attenuation * lightIntensity;
    }
    
    // Ensure the result doesn't exceed maximum brightness
    result = min(result, vec3(1.0));
    
    FragColor = vec4(result, 1.0);
}