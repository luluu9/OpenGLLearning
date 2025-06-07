#version 330 core
// Description: Fragment shader for chrome/reflective material effect

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 ReflectDir;
} fs_in;

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

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// Chrome properties (using constant values)
const float REFLECTIVITY = 0.8;     // How reflective the surface is
const float FRESNEL_FACTOR = 5.0;   // Edge reflection strength
const vec3 CHROME_COLOR = vec3(0.8, 0.8, 0.9); // Base color tint

// Camera position for fresnel calculation
uniform vec3 viewPos;

void main()
{
    // Using constant values defined at the top
    float reflectivityValue = REFLECTIVITY;
    float fresnelFactorValue = FRESNEL_FACTOR;
    vec3 chromeColorValue = CHROME_COLOR;
      // Normalize vectors
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    // Fresnel effect (edges are more reflective)
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), fresnelFactorValue);
    
    // Fake environment reflection using normals and view direction
    vec3 reflection = normalize(fs_in.ReflectDir);
    vec3 fakeCubeMapColor = abs(reflection); // Simple fake reflection color
    
    // Initialize lighting components
    vec3 ambient = material.ambient;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    
    // Process each light
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        // Get light properties
        vec3 lightPos = lights[i].position;
        vec3 lightColor = lights[i].color;
        float lightIntensity = lights[i].intensity;
        
        // Calculate light direction
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        
        // Calculate distance attenuation
        float distance = length(lightPos - fs_in.FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse += diff * material.diffuse * lightColor * attenuation * lightIntensity;
        
        // Specular (more intense for chrome)
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        specular += spec * material.specular * lightColor * 2.0 * attenuation * lightIntensity;  // Boosted for chrome effect
    }
    
    // Combine chrome components
    vec3 chromeEffect = chromeColorValue * (ambient * 0.2 + diffuse) + specular;
    vec3 reflectionEffect = fakeCubeMapColor * (reflectivityValue + fresnel);
    
    // Blend between chrome and reflection
    vec3 result = mix(chromeEffect, reflectionEffect, reflectivityValue);
    
    // Output final color
    FragColor = vec4(result, 1.0);
}
