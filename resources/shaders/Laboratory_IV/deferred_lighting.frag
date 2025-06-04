#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

// G-buffer textures
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;
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
    
    // Diffuse component
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = lightIntensity * diff * lightColor * Albedo;
    
    // Specular component (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess);
    vec3 specular = lightIntensity * spec * lightColor * vec3(SpecularIntensity);
    
    // Attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    // Apply attenuation to diffuse and specular, but not ambient
    diffuse *= attenuation;
    specular *= attenuation;
    
    // Final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}
