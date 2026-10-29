#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvValue;

uniform sampler2D heightMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 scene;

uniform int useHeight;

out vec4 screenPos;
out vec2 uv;

void main()
{
	uv = uvValue;
	vec3 pos = aPos;
	if(useHeight==1)
	{
		ivec2 textureDimensions = textureSize(heightMap, 0);

		ivec2 texel = ivec2(round(
			uvValue * vec2(textureDimensions - ivec2(1))
		));

		// Retrieve the exact height assigned to this vertex.
		pos.y = texelFetch(heightMap, texel, 0).r;
	}
	screenPos = gl_Position = projection * view * scene * model * vec4(pos, 1.0);
}