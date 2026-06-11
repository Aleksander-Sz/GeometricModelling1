#version 410 core

layout (vertices = 20) out;

in vec3 vertexPos[];
out vec3 tcsPos[];

uniform float tessLevel;

void main()
{
	tcsPos[gl_InvocationID] =
        vertexPos[gl_InvocationID];

    if(gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;

        gl_TessLevelInner[0] = tessLevel;
    }
}