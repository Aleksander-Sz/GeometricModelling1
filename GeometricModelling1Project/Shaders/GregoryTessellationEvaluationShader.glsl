#version 410 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 model;
uniform mat4 scene;
uniform mat4 view;
uniform mat4 projection;

in vec3 tcsPos[];

out vec2 uv;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    uv = vec2(u, v);

    //
    // corners
    //
    vec3 V0 = tcsPos[0];
    vec3 V1 = tcsPos[1];
    vec3 V2 = tcsPos[2];

    //
    // cubic boundary controls
    //
    vec3 E01 = tcsPos[3];
    vec3 E10 = tcsPos[4];

    vec3 E12 = tcsPos[5];
    vec3 E21 = tcsPos[6];

    vec3 E20 = tcsPos[7];
    vec3 E02 = tcsPos[8];

    //
    // tangents
    //
    vec3 T01 = tcsPos[9];
    vec3 T10 = tcsPos[10];

    vec3 T12 = tcsPos[11];
    vec3 T21 = tcsPos[12];

    vec3 T20 = tcsPos[13];
    vec3 T02 = tcsPos[14];

    //
    // cross-boundary derivatives
    //
    vec3 D01 = tcsPos[15];
    vec3 D10 = tcsPos[16];

    vec3 D12 = tcsPos[17];
    vec3 D21 = tcsPos[18];

    vec3 D20 = tcsPos[19];
    vec3 D02 = tcsPos[20];

    //
    // temporary Gregory interior data
    //
    vec3 F01 = 0.5 * (T01 + D10);
    vec3 F12 = 0.5 * (T12 + D21);
    vec3 F20 = 0.5 * (T20 + D02);

    vec3 P111 = (F01 + F12 + F20) / 3.0;

    //
    // cubic triangular Bernstein basis
    //
    float B300 = u*u*u;
    float B030 = v*v*v;
    float B003 = w*w*w;

    float B210 = 3.0 * u*u * v;
    float B120 = 3.0 * u * v*v;

    float B021 = 3.0 * v*v * w;
    float B012 = 3.0 * v * w*w;

    float B102 = 3.0 * u * w*w;
    float B201 = 3.0 * u*u * w;

    float B111 = 6.0 * u * v * w;

    //
    // cubic Bézier triangle evaluation
    //
    vec3 position =
          B300 * V0
        + B030 * V1
        + B003 * V2

        + B210 * E01
        + B120 * E10

        + B021 * E12
        + B012 * E21

        + B201 * E02
        + B102 * E20

        + B111 * P111;

    gl_Position =
        projection *
        view *
        scene *
        model *
        vec4(position, 1.0);
}