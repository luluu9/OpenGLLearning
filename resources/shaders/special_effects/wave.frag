#version 330 core
// Description: Fragment shader for wave distortion effect

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    float WaveDisplacement;
} fs_in;

out vec4 FragColor;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

const vec3 WAVE_COLOR = vec3(0.0, 0.5, 1.0);
const float WAVE_COLOR_INTENSITY = 0.5;

void main()
{
    vec3 waveColorValue = WAVE_COLOR;
    float waveColorIntensityValue = WAVE_COLOR_INTENSITY;

    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    float normalStrength = 0.5;
    vec3 adjustedNormal = normalize(normal + vec3(0.0, fs_in.WaveDisplacement * normalStrength, 0.0));
    
    // Use displacement for wave color effect
    float localWaveAmplitude = 0.1; 
    float waveEffect = (fs_in.WaveDisplacement + localWaveAmplitude) / (2.0 * localWaveAmplitude); 
    
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
        
        // Diffuse with wave color effect
        float diff = max(dot(adjustedNormal, lightDir), 0.0);
        vec3 baseColor = material.diffuse;
        vec3 colorWithWave = mix(baseColor, waveColorValue, waveEffect * waveColorIntensityValue);
        
        // Apply wave coloring to diffuse component
        diffuse += diff * colorWithWave * lightColor * attenuation * lightIntensity;
        
        // Specular
        float spec = pow(max(dot(adjustedNormal, halfwayDir), 0.0), material.shininess);
        specular += spec * material.specular * lightColor * attenuation * lightIntensity;
    }
    
    // Combine all components
    vec3 result = ambient * 0.2 + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
}
