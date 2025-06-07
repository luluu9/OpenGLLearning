#version 410 core

layout (vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec2 vTexCoord[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out vec2 tcTexCoord[];

uniform float tessLevelOuter;
uniform float tessLevelInner;

void main()
{
    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
    
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = tessLevelOuter;
        gl_TessLevelOuter[1] = tessLevelOuter;
        gl_TessLevelOuter[2] = tessLevelOuter;
        
        gl_TessLevelInner[0] = tessLevelInner;
    }
}
