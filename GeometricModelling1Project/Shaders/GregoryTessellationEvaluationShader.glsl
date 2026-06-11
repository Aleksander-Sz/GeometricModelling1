#version 410 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 model;
uniform mat4 scene;
uniform mat4 view;
uniform mat4 projection;

in vec3 tcsPos[];

out vec2 uv;

vec3 Bezier(
    float t,
    vec3 p0,
    vec3 p1,
    vec3 p2,
    vec3 p3)
{
    vec3 a = mix(p0, p1, t);
    vec3 b = mix(p1, p2, t);
    vec3 c = mix(p2, p3, t);

    vec3 d = mix(a, b, t);
    vec3 e = mix(b, c, t);

    return mix(d, e, t);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    uv = vec2(u, v);

    float eps = 0.00001f;
    vec3 BezierPoints[16];

    BezierPoints[0] = tcsPos[0];
    BezierPoints[1] = tcsPos[1];
    BezierPoints[2] = tcsPos[2];
    BezierPoints[3] = tcsPos[3];
    BezierPoints[4] = tcsPos[4];
    BezierPoints[5] =
    (u * tcsPos[5] + v * tcsPos[6])
    / max(u + v, eps);
    BezierPoints[6] =
    (u * tcsPos[7] + (1.0 - v) * tcsPos[8])
    / max(u + (1.0 - v), eps);
    BezierPoints[7] = tcsPos[9];
    BezierPoints[8] = tcsPos[10];
    BezierPoints[9] =
    ((1.0 - u) * tcsPos[11] + v * tcsPos[12])
    / max((1.0 - u) + v, eps);
    BezierPoints[10] =
    ((1.0 - u) * tcsPos[13] + (1.0 - v) * tcsPos[14])
    / max((1.0 - u) + (1.0 - v), eps);
    BezierPoints[11] = tcsPos[15];
    BezierPoints[12] = tcsPos[16];
    BezierPoints[13] = tcsPos[17];
    BezierPoints[14] = tcsPos[18];
    BezierPoints[15] = tcsPos[19];

    vec3 curve0 = Bezier(u,
        BezierPoints[0],
        BezierPoints[1],
        BezierPoints[2],
        BezierPoints[3]);

    vec3 curve1 = Bezier(u,
        BezierPoints[4],
        BezierPoints[5],
        BezierPoints[6],
        BezierPoints[7]);

    vec3 curve2 = Bezier(u,
        BezierPoints[8],
        BezierPoints[9],
        BezierPoints[10],
        BezierPoints[11]);

    vec3 curve3 = Bezier(u,
        BezierPoints[12],
        BezierPoints[13],
        BezierPoints[14],
        BezierPoints[15]);

    vec3 position = Bezier(v,
        curve0,
        curve1,
        curve2,
        curve3);

    gl_Position =
        projection *
        view *
        scene *
        model *
        vec4(position, 1.0);
}