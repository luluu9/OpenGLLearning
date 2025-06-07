#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

#define MAX_LIGHTS 10
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

void main()
{
    int debugMode = 0;

    if (debugMode == 1) {
        vec3 Albedo = texture(gAlbedoSpec, TexCoord).rgb;
        FragColor = vec4(Albedo, 1.0);
        return;
    } 
    else if (debugMode == 2) {
        vec3 Normal = texture(gNormal, TexCoord).rgb;
        Normal = Normal * 0.5 + 0.2;
        FragColor = vec4(Normal, 1.0);
        return;
    }
    else if (debugMode == 3) {
        vec3 FragPos = texture(gPosition, TexCoord).rgb;
        FragPos = FragPos * 0.1 + 0.2;
        FragColor = vec4(FragPos, 1.0);
        return;
    }

    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoord).rgb);
    vec3 Albedo = texture(gAlbedoSpec, TexCoord).rgb;
    float SpecularIntensity = texture(gAlbedoSpec, TexCoord).a;
    float Shininess = max(SpecularIntensity * 256.0, 1.0);

    vec3 ambient = 0.2 * Albedo;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 lightDiffuse = lights[i].intensity * diff * lights[i].color * Albedo;
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), Shininess);
        vec3 lightSpecular = lights[i].intensity * spec * lights[i].color * vec3(SpecularIntensity);
        diffuse += lightDiffuse * attenuation;
        specular += lightSpecular * attenuation;
    }

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
