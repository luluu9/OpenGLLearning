#version 330 core
// Description: Vertex shader for Phong shading (all lighting calculated in fragment shader)

// Inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Outputs to fragment shader
out vec3 FragPos;           // Fragment position in world space
out vec3 Normal;            // Normal in world space
out vec3 LightDir;          // Direction to light in world space
out vec3 ViewDir;           // Direction to viewer in world space
out vec2 TexCoords;         // Texture coordinates

// Uniforms
uniform mat4 model;         // Model matrix
uniform mat4 view;          // View matrix
uniform mat4 projection;    // Projection matrix
uniform vec3 lightPos;      // Light position in world space
uniform vec3 viewPos;       // View/camera position in world space

void main()
{
    // Transform vertex position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space (excluding non-uniform scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Calculate light direction vector in world space (will normalize in fragment shader)
    LightDir = lightPos - FragPos;
    
    // Calculate view direction vector in world space (will normalize in fragment shader)
    ViewDir = viewPos - FragPos;
    
    // Pass texture coordinates
    TexCoords = aTexCoords;
    
    // Transform vertex to clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
