#version 410 core

layout (location = 0) in vec3 aPos;

//uniform mat4 model;
//uniform mat4 scene;

out vec3 vertexPos;

void main()
{
	//vec4 transformedPos = /*scene * */ model * vec4(aPos, 1.0);
    //vertexPos = vec3(transformedPos);
	gl_Position = vec4(aPos, 1.0);
}