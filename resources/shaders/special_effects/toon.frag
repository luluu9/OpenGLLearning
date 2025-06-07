#version 330 core
// Description: Fragment shader for toon/cel shading effect with edge detection

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 FragColor;

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

const int levels = 4;
const float scaleFactor = 1.0 / levels;

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float edge = dot(normal, viewDir);
    float edgeFactor = edge < 0.3 ? 0.0 : 1.0;
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
        float diffStrength = max(dot(normal, lightDir), 0.0);
        diffStrength = floor(diffStrength * levels) * scaleFactor;
        diffuse += diffStrength * material.diffuse * lightColor * attenuation * lightIntensity;

        if (diffStrength > 0.0) {
            float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
            float specStrength = spec > 0.3 ? 1.0 : 0.0;
            specular += specStrength * material.specular * lightColor * attenuation * lightIntensity;
        }
    }

    vec3 result = (ambient * 0.2 + diffuse + specular) * edgeFactor;
    FragColor = vec4(result, 1.0);
}
