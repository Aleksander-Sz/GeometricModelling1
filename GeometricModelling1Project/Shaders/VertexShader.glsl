#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 scene;

out vec4 screenPos;

void main()
{
	screenPos = gl_Position = projection * view * scene * model * vec4(aPos, 1.0);
}