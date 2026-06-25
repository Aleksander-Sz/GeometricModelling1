#version 330 core

uniform vec3 color;
uniform float tessLevelU;
uniform float tessLevelV;
uniform int clip;
uniform sampler2D clipTex;

in vec2 uvGlobal;
in vec2 uvLocal;

out vec4 FragColor;

void main()
{
    if(clip>=1)
	{
        float clipSignal = texture(clipTex, uvGlobal.yx).r;
        if(clip==2)
        {
            clipSignal = 1.0f - clipSignal;
        }
		if(clipSignal < 0.5f)
		{
			discard;
		}
	}

    vec2 grid = vec2(
        uvLocal.x * tessLevelU,
        uvLocal.y * tessLevelV
    );

    vec2 dist = abs(grid - round(grid));

    float lineWidth = 0.012;

    bool line =
        dist.x < lineWidth ||
        dist.y < lineWidth;

    if(line)
        FragColor = vec4(color, 1.0f);
    else
        discard;
    //FragColor = vec4(uvGlobal.x, uvGlobal.y, 0.0f, 1.0f);
    //if(uvGlobal.x>0.99||uvGlobal.y>0.99)
    //    FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
}