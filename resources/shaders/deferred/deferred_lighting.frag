#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

// G-buffer textures
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

// Light properties
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

void main()
{
    // Choose which debug mode to use (0=none, 1=albedo, 2=normals, 3=positions)
    int debugMode = 0;
    
    if (debugMode == 1) {
        // Debug 1: Just show the albedo color
        vec3 Albedo = texture(gAlbedoSpec, TexCoord).rgb;
        FragColor = vec4(Albedo, 1.0);
        return;
    } 
    else if (debugMode == 2) {
        // Debug 2: Show normals
        vec3 Normal = texture(gNormal, TexCoord).rgb;
        Normal = Normal * 0.5 + 0.2; // Transform from [-1,1] to [0,1]
        FragColor = vec4(Normal, 1.0);
        return;
    }
    else if (debugMode == 3) {
        // Debug 3: Show positions
        vec3 FragPos = texture(gPosition, TexCoord).rgb;
        FragPos = FragPos * 0.1 + 0.2; // Scale to make visible
        FragColor = vec4(FragPos, 1.0);
        return;
    }
    
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoord).rgb); // Ensure normalized
    vec3 Albedo = texture(gAlbedoSpec, TexCoord).rgb;
    float SpecularIntensity = texture(gAlbedoSpec, TexCoord).a;
    float Shininess = max(SpecularIntensity * 256.0, 1.0); // Ensure minimum shininess
      // Ambient component - base lighting
    vec3 ambient = 0.2 * Albedo;
    
    // Initialize diffuse and specular components to zero
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Process each light
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        // Calculate light direction and distance for current light
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        
        // Calculate attenuation
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Diffuse component for current light
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 lightDiffuse = lights[i].intensity * diff * lights[i].color * Albedo;
        
        // Specular component (Blinn-Phong) for current light
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess);
        vec3 lightSpecular = lights[i].intensity * spec * lights[i].color * vec3(SpecularIntensity);
        
        // Apply attenuation and accumulate
        diffuse += lightDiffuse * attenuation;
        specular += lightSpecular * attenuation;
    }
    
    // Final color - combine ambient with accumulated diffuse and specular
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}
