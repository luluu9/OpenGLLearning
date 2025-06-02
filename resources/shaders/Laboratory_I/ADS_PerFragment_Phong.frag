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

// Light properties
uniform vec3 lightColor;
uniform float lightIntensity;

// Light properties
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

void main()
{
    // Normalize the interpolated vectors (they lose normalization during interpolation)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(LightDir);
    vec3 viewDir = normalize(ViewDir);
    
    // Calculate ambient component
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * material.ambient;
    
    // Calculate diffuse intensity
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    
    // Calculate diffuse component
    vec3 diffuseColor = diffuseFactor * material.diffuse;
    
    // Initialize result with ambient and diffuse components
    vec3 result = ambientColor + diffuseColor;
    
    // Only calculate specular component if diffuse factor is greater than zero
    if (diffuseFactor > 0.0) {
        // Calculate reflection vector
        vec3 reflectionVector = reflect(-lightDir, normal);
        
        // Calculate specular factor
        float specularFactor = pow(max(dot(viewDir, reflectionVector), 0.0), material.shininess);
        
        // Add specular component to result
        result += specularFactor * material.specular;
    }
    
    // Set fragment color with light color and intensity
    FragColor = vec4(result * lightColor * lightIntensity, 1.0);
}
