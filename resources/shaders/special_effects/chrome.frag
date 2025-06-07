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

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

const float REFLECTIVITY = 0.8;
const float FRESNEL_FACTOR = 5.0;
const vec3 CHROME_COLOR = vec3(0.8, 0.8, 0.9);
uniform vec3 viewPos;

void main()
{
    float reflectivityValue = REFLECTIVITY;
    float fresnelFactorValue = FRESNEL_FACTOR;
    vec3 chromeColorValue = CHROME_COLOR;
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), fresnelFactorValue);
    vec3 reflection = normalize(fs_in.ReflectDir);
    vec3 fakeCubeMapColor = abs(reflection);
    vec3 ambient = material.ambient;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 lightPos = lights[i].position;
        vec3 lightColor = lights[i].color;
        float lightIntensity = lights[i].intensity;
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float distance = length(lightPos - fs_in.FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse += diff * material.diffuse * lightColor * attenuation * lightIntensity;
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        specular += spec * material.specular * lightColor * 2.0 * attenuation * lightIntensity;
    }

    vec3 chromeEffect = chromeColorValue * (ambient * 0.2 + diffuse) + specular;
    vec3 reflectionEffect = fakeCubeMapColor * (reflectivityValue + fresnel);
    vec3 result = mix(chromeEffect, reflectionEffect, reflectivityValue);
    FragColor = vec4(result, 1.0);
}
