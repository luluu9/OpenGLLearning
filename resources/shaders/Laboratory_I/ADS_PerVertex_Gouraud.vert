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

// Light properties
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

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
    
    // Transform light position to eye space
    vec3 eyeLightPos = vec3(view * vec4(lightPos, 1.0));
    
    // Calculate light vector in eye space
    vec3 lightVector = normalize(eyeLightPos - eyePositionVec3);
    
    // Calculate Lambert diffuse term
    float diffuseFactor = max(dot(eyeNormal, lightVector), 0.0);
    
    // Calculate ambient component
    float ambientStrength = 0.1;
    vec3 ambientColor = ambientStrength * material.ambient;
    
    // Calculate diffuse component
    vec3 diffuseColor = diffuseFactor * material.diffuse;
    
    // Initialize specular component
    vec3 specularColor = vec3(0.0);
    
    // Only calculate specular component if diffuse factor is greater than zero
    if (diffuseFactor > 0.0) {
        // Calculate view vector (from vertex to eye, which is at origin in eye space)
        vec3 viewVector = normalize(-eyePositionVec3);
        
        // Calculate reflection vector
        vec3 reflectionVector = reflect(-lightVector, eyeNormal);
        
        // Calculate specular factor
        float specularFactor = pow(max(dot(reflectionVector, viewVector), 0.0), material.shininess);
        
        // Calculate specular component
        specularColor = specularFactor * material.specular;
    }
    
    // Set output color with all components
    vColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Transform vertex to clip space
    gl_Position = projection * view * vec4(worldPos, 1.0);
}
