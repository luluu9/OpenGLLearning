#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

// Light properties
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

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
    vec3 norm = normalize(Normal);
    vec3 ambient = material.ambient;
    vec3 result = vec3(0.0);
    if (lightingModel == 0)
    {
        vec3 diffuse = vec3(0.0);
        
        // Process each light
        for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
            // Calculate light direction
            vec3 lightDir = normalize(lights[i].position - FragPos);
            
            // Simple diffuse calculation without interpolated normals
            float diff = max(dot(norm, lightDir), 0.0);
            
            // Simple distance attenuation
            float distance = length(lights[i].position - FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            
            // For flat shading, we use a simpler formula without specular
            vec3 lightDiffuse = diff * material.diffuse * lights[i].color;
            
            // Combine lighting with flat appearance (no specular)
            diffuse += lightDiffuse * attenuation * lights[i].intensity;
        }
        
        result = ambient * 0.2 + diffuse;
    }
    else if (lightingModel == 1) // Blinn-Phong lighting
    {
        // Initialize lighting results
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // Process each light
        for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
            // Calculate light direction and distance
            vec3 lightDir = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            
            // Calculate diffuse component
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 lightDiffuse = diff * material.diffuse * lights[i].color;
            
            // Calculate specular component (Blinn-Phong)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            vec3 lightSpecular = spec * material.specular * lights[i].color;
            
            // Combine all lighting components with attenuation and intensity
            diffuse += lightDiffuse * attenuation * lights[i].intensity;
            specular += lightSpecular * attenuation * lights[i].intensity;
        }
        
        result = ambient * 0.2 + diffuse + specular;
    }
    
    // Ensure the result doesn't exceed maximum brightness
    result = min(result, vec3(1.0));
    
    FragColor = vec4(result, 1.0);
}
