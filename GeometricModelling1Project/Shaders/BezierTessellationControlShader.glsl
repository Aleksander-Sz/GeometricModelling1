#version 410 core

layout (vertices = 4) out;

in vec3 vertexPos[];
out vec3 tcsPos[];

void main()
{
	tcsPos[gl_InvocationID] = vertexPos[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID==0)
	{
		float tessLevel = 16.0;
		gl_TessLevelOuter[0] = 1.0; // not used for isolines
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelInner[0] = tessLevel; // just for redundancy with different drivers
	}
}