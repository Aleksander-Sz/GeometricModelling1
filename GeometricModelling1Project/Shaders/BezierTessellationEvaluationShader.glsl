#version 410 core

layout (isolines, equal_spacing) in;

uniform mat4 model;
uniform mat4 scene;
uniform mat4 view;
uniform mat4 projection;

in vec3 tcsPos[];

vec3 Bezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
	float u = 1.0 - t;
	return u*u*u*p0 + 3.0*u*u*t*p1 + 3.0*u*t*t*p2 + t*t*t*p3;
}

void main()
{
	float t = gl_TessCoord.x;

	vec3 p0 = tcsPos[0].xyz;
	vec3 p1 = tcsPos[1].xyz;
	vec3 p2 = tcsPos[2].xyz;
	vec3 p3 = tcsPos[3].xyz;

	//p1 = vec3(0.0, 1.0, 0.0);
	//p2 = vec3(2.0, 1.5, 0.0);

	vec3 position = Bezier(t, p0, p1, p2, p3);

	//gl_Position = vec4(t * 2.0 - 1.0, t*t, 0.0, 1.0);
	gl_Position = projection * view * scene * model * vec4(position, 1.0f);
	//if(t<0.5)
	//	gl_Position = projection * view * vec4(-5.0, 0.0, 1.0, 1.0);
	//else
	//	gl_Position = projection * view * vec4(0.0, 5.5, 0.0, 1.0);
}