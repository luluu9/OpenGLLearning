#version 330 core
// Description: Fragment shader for Phong shading (all lighting calculated per fragment)

// Inputs from vertex shader
in vec3 FragPos;            // Fragment position in world space
in vec3 Normal;             // Normal in world space
in vec3 LightDir;           // Direction to light in world space
in vec3 ViewDir;            // Direction to viewer in world space
in vec2 TexCoords;          // Texture coordinates

// Output
out vec4 FragColor;

// Material properties
uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

// Light properties
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform float time; // Global time for animations

void main()
{
    // Normalize the interpolated vectors (they lose normalization during interpolation)
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(ViewDir);
    
    // Calculate ambient component
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * material.ambient;
    
    // Initialize diffuse and specular components
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);
    
    // Process all lights
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        // Calculate light direction for this light
        vec3 lightDir = normalize(lights[i].position - FragPos);
        
        // Calculate diffuse intensity
        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        
        // Calculate distance attenuation
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Add diffuse contribution from this light
        diffuseColor += diffuseFactor * material.diffuse * lights[i].color * lights[i].intensity * attenuation;
        
        // Only calculate specular component if diffuse factor is greater than zero
        if (diffuseFactor > 0.0) {
            // Calculate reflection vector
            vec3 reflectionVector = reflect(-lightDir, normal);
            
            // Calculate specular factor
            float specularFactor = pow(max(dot(viewDir, reflectionVector), 0.0), material.shininess);
            
            // Add specular contribution from this light
            specularColor += specularFactor * material.specular * lights[i].color * lights[i].intensity * attenuation;
        }
    }
      // Combine all lighting components
    vec3 result = ambientColor + diffuseColor + specularColor;
    
    // Set fragment color (light color and intensity already applied in the lighting calculations)
    FragColor = vec4(result, 1.0);
}
