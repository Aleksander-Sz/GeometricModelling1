#version 330 core

uniform vec3 color;
uniform int clip;
uniform sampler2D clipTex;

in vec2 uv;

out vec4 FragColor;

void main()
{
	if(clip>=1)
	{
        float clipSignal = texture(clipTex, uv).r;
        if(clip==2)
        {
            clipSignal = 1.0f - clipSignal;
        }
		if(clipSignal < 0.5f)
		{
			discard;
		}
	}
	FragColor = vec4(color, 1.0f);
}