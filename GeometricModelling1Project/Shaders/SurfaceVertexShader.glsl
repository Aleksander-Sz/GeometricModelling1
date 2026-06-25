#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvIn;

out vec3 vertexPos;
out vec2 uv;

void main()
{
	uv = uvIn;
	vertexPos = aPos;
	gl_Position = vec4(aPos, 1.0);
}