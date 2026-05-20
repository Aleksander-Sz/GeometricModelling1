#version 330 core

uniform vec3 color;
uniform float tessLevel;

in vec2 uv;

out vec4 FragColor;

void main()
{
    vec2 grid = uv * tessLevel;

    vec2 dist = abs(grid - round(grid));

    float lineWidth = 0.008;

    bool line =
        dist.x < lineWidth ||
        dist.y < lineWidth;

    if(line)
	    FragColor = vec4(color, 1.0f);
    else
        discard;
}