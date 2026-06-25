#version 410 core

layout (vertices = 16) out;

in vec3 vertexPos[];
in vec2 uv[];
out vec3 tcsPos[];
out vec2 uvBase[];

uniform float tessLevelU;
uniform float tessLevelV;

void main()
{
	tcsPos[gl_InvocationID] = vertexPos[gl_InvocationID];
    uvBase[gl_InvocationID] = uv[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = tessLevelV;
        gl_TessLevelOuter[1] = tessLevelU;
        gl_TessLevelOuter[2] = tessLevelV;
        gl_TessLevelOuter[3] = tessLevelU;

        gl_TessLevelInner[0] = tessLevelU;
        gl_TessLevelInner[1] = tessLevelV;

    }
}