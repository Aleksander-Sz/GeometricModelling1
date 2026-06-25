#version 410 core

layout (isolines, equal_spacing) in;

uniform mat4 model;
uniform mat4 scene;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 tRange;

in vec3 tcsPos[];

out vec2 uv;

vec3 Bezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
	vec3 v01 = mix(p0, p1, t);
	vec3 v11 = mix(p1, p2, t);
	vec3 v21 = mix(p2, p3, t);

	vec3 v02 = mix(v01, v11, t);
	vec3 v12 = mix(v11, v21, t);

	return mix(v02, v12, t);
}

void main()
{
	float t = gl_TessCoord.x;

	vec3 p0 = tcsPos[0].xyz;
	vec3 p1 = tcsPos[1].xyz;
	vec3 p2 = tcsPos[2].xyz;
	vec3 p3 = tcsPos[3].xyz;

	vec3 position = Bezier(mix(tRange.x, tRange.y, t), p0, p1, p2, p3);

	gl_Position = projection * view * scene * model * vec4(position, 1.0f);
}