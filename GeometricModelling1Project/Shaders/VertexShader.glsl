#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 scene;

out vec4 screenPos;
out vec2 uv;

void main()
{
	uv = uvValue;
	screenPos = gl_Position = projection * view * scene * model * vec4(aPos, 1.0);
}