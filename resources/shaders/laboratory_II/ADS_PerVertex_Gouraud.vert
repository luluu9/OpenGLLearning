#version 330 core
// Description: Gouraud shading with ambient, diffuse, and specular components (per-vertex)

// Inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Outputs
out vec4 vColor;  // Output color from vertex shader to fragment shader
out vec2 TexCoords;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;  // Light position in world space
uniform vec3 viewPos;   // View/camera position in world space

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
    // Transform vertex position to world space
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space and normalize
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    
    // Calculate view matrix * model matrix (for eye space transformation)
    mat4 viewModel = view * model;
    
    // Transform position to eye space
    vec4 eyePosition = viewModel * vec4(aPos, 1.0);
    vec3 eyePositionVec3 = eyePosition.xyz;
    
    // Transform normal to eye space and normalize
    vec3 eyeNormal = normalize(mat3(transpose(inverse(viewModel))) * aNormal);
    
    // Initialize lighting components
    vec3 ambientColor = vec3(0.1) * material.ambient; // Base ambient
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);
    
    // Process all lights
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        // Transform light position to eye space
        vec3 eyeLightPos = vec3(view * vec4(lights[i].position, 1.0));
        
        // Calculate light vector in eye space
        vec3 lightVector = normalize(eyeLightPos - eyePositionVec3);
        
        // Calculate Lambert diffuse term
        float diffuseFactor = max(dot(eyeNormal, lightVector), 0.0);
        
        // Calculate light attenuation based on distance
        float distance = length(eyeLightPos - eyePositionVec3);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Add diffuse contribution from this light
        diffuseColor += diffuseFactor * material.diffuse * lights[i].color * attenuation * lights[i].intensity;
          // Only calculate specular if diffuse factor is > 0
        if (diffuseFactor > 0.0) {
            // Calculate view vector (from vertex to eye, which is at origin in eye space)
            vec3 viewVector = normalize(-eyePositionVec3);
            
            // Calculate reflection vector
            vec3 reflectionVector = reflect(-lightVector, eyeNormal);
            
            // Calculate specular factor
            float specularFactor = pow(max(dot(reflectionVector, viewVector), 0.0), material.shininess);
            
            // Add specular contribution from this light
            specularColor += specularFactor * material.specular * lights[i].color * attenuation * lights[i].intensity;
        }
    }
    
    // Set output color with all components
    vColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Transform vertex to clip space
    gl_Position = projection * view * vec4(worldPos, 1.0);
}
