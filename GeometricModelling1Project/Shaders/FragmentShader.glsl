#version 330 core

uniform vec3 color;
uniform int clip;
uniform sampler2D clipTex;

in vec2 uv;

out vec4 FragColor;

void main()
{
	if(false&&clip==1)
	{
		if(texture(clipTex, uv).r > 0.5f);
			discard;
	}
	FragColor = vec4(color, 1.0f);
}