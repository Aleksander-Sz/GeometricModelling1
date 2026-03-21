#version 330 core

layout (location = 0) in int index;

uniform vec2 corner[2];

out vec4 screenPos;

void main()
{
	int a = index%2;
	int b = index/2;
	vec4 pointPosition = vec4(corner[a].x, corner[b].y, 0.0f, 1.0f);
	screenPos = gl_Position = pointPosition;
}