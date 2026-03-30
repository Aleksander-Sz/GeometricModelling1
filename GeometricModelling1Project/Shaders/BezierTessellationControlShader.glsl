#version 410 core

layout (vertices = 4) out;

in vec3 vertexPos[];
out vec3 tcsPos[];

uniform mat4 model;
uniform mat4 scene;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 viewportSize;

void main()
{
	tcsPos[gl_InvocationID] = vertexPos[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID==0)
	{
		// Calculating the line length in pixels

		float screenWidth = viewportSize.x;
		float screenHeight = viewportSize.y;
		mat4 viewportTransformationMatrix = mat4(
			screenWidth / 2.0, 0.0, 0.0, screenWidth / 2.0,
			0.0, -screenHeight / 2.0, 0.0, screenHeight / 2.0,
			0.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 1.0
		);
		
		vec4 point0 = viewportTransformationMatrix * projection * view * scene * model * vec4(vertexPos[0], 1.0);
		vec4 point1 = viewportTransformationMatrix * projection * view * scene * model * vec4(vertexPos[1], 1.0);
		vec4 point2 = viewportTransformationMatrix * projection * view * scene * model * vec4(vertexPos[2], 1.0);
		vec4 point3 = viewportTransformationMatrix * projection * view * scene * model * vec4(vertexPos[3], 1.0);

		float l1 = length(point1.xy - point0.xy);
		float l2 = length(point2.xy - point1.xy);
		float l3 = length(point3.xy - point2.xy);

		float tessLevel = l1 + l2 + l3;
		//tessLevel /= 20.0;

		gl_TessLevelOuter[0] = 1.0; // not used for isolines
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelInner[0] = tessLevel; // just for redundancy with different drivers

	}
}