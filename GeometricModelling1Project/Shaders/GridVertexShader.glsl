#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 gridCoords;
out vec4 worldPos;

void main()
{
	vec4 worldSpacePosition =  model * vec4(aPos, 1.0f);
	vec4 cameraSpacePosition = view * worldSpacePosition;
	gl_Position = projection * cameraSpacePosition;
	gridCoords = (worldSpacePosition).xz * 10;
	worldPos = worldSpacePosition;
}