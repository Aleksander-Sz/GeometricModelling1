#version 330 core

in vec2 gridCoords;
in float distanceFromCamera;
in vec4 worldPos;

uniform mat4 view;

out vec4 FragColor;

void main()
{
    float minorsPerMajor = 5.0;

    float distanceFromCamera = length(view * worldPos);
    vec2 coord = gridCoords;

    // Anti-aliased grid distance
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);

    // Minor lines
    float minorLine = min(grid.x, grid.y);

    // Major lines
    vec2 majorCoord = coord / minorsPerMajor;
    vec2 majorGrid = abs(fract(majorCoord - 0.5) - 0.5) / fwidth(majorCoord);
    float majorLine = min(majorGrid.x, majorGrid.y);

    float line = min(minorLine, majorLine);

    vec3 color = vec3(0.5);
    float fadeStart = 1.0;
    float fadeStop = 10.0;

    // Major thicker lines
    if (majorLine < minorLine)
    {
        color = vec3(1.0);
        fadeStart = 1.0;
        fadeStop = 20.0;
    }

    // Axis lines
    if (abs(coord.x) < fwidth(coord.x))
    {
        color = vec3(1,0,0);
        fadeStart = 1.0;
        fadeStop = 30.0;
    }

    if (abs(coord.y) < fwidth(coord.y))
    {
        color = vec3(0,1,0);
        fadeStart = 2.0;
        fadeStop = 30.0;
    }

    float alpha = 1.0 - clamp(line, 0.0, 1.0);

    if (alpha <= 0.01)
        discard;
    float fade = 1.0 - smoothstep(fadeStart,fadeStop,distanceFromCamera);
    alpha *= fade;
    FragColor = vec4(color, alpha);
}
