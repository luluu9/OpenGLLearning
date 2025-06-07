#version 410 core

layout (triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];
in vec2 tcTexCoord[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float displaceAmount;

float getHeight(vec2 uv)
{
    float height = sin(uv.x * 10.0) * cos(uv.y * 10.0) * 0.5;
    return height * displaceAmount;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    
    vec3 position = tcPosition[0] * u + tcPosition[1] * v + tcPosition[2] * w;
    vec3 normal = normalize(tcNormal[0] * u + tcNormal[1] * v + tcNormal[2] * w);
    vec2 texCoord = tcTexCoord[0] * u + tcTexCoord[1] * v + tcTexCoord[2] * w;
    
    position += normal * getHeight(texCoord);
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
