#version 330 core

uniform vec3 color;
uniform float tessLevelU;
uniform float tessLevelV;

in vec2 uv;

out vec4 FragColor;

void main()
{
    vec2 grid = vec2(
        uv.x * tessLevelU,
        uv.y * tessLevelV
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
}