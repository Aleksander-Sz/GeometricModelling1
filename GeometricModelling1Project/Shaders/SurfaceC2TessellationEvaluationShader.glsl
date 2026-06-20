#version 410 core

layout(quads, equal_spacing, ccw) in;

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
    uv = gl_TessCoord.xy;

    float u = uv.x;
    float v = uv.y;

    // Convert rows from de Boor -> Bernstein

    vec3 r00 = (tcsPos[0]  + 4.0*tcsPos[1]  + tcsPos[2])  / 6.0;
    vec3 r01 = (4.0*tcsPos[1]  + 2.0*tcsPos[2]) / 6.0;
    vec3 r02 = (2.0*tcsPos[1]  + 4.0*tcsPos[2]) / 6.0;
    vec3 r03 = (tcsPos[1]  + 4.0*tcsPos[2]  + tcsPos[3])  / 6.0;

    vec3 r10 = (tcsPos[4]  + 4.0*tcsPos[5]  + tcsPos[6])  / 6.0;
    vec3 r11 = (4.0*tcsPos[5]  + 2.0*tcsPos[6]) / 6.0;
    vec3 r12 = (2.0*tcsPos[5]  + 4.0*tcsPos[6]) / 6.0;
    vec3 r13 = (tcsPos[5]  + 4.0*tcsPos[6]  + tcsPos[7])  / 6.0;

    vec3 r20 = (tcsPos[8]  + 4.0*tcsPos[9]  + tcsPos[10]) / 6.0;
    vec3 r21 = (4.0*tcsPos[9]  + 2.0*tcsPos[10]) / 6.0;
    vec3 r22 = (2.0*tcsPos[9]  + 4.0*tcsPos[10]) / 6.0;
    vec3 r23 = (tcsPos[9]  + 4.0*tcsPos[10] + tcsPos[11]) / 6.0;

    vec3 r30 = (tcsPos[12] + 4.0*tcsPos[13] + tcsPos[14]) / 6.0;
    vec3 r31 = (4.0*tcsPos[13] + 2.0*tcsPos[14]) / 6.0;
    vec3 r32 = (2.0*tcsPos[13] + 4.0*tcsPos[14]) / 6.0;
    vec3 r33 = (tcsPos[13] + 4.0*tcsPos[14] + tcsPos[15]) / 6.0;

    // Convert columns from de Boor -> Bernstein

    vec3 b00 = (r00 + 4.0*r10 + r20) / 6.0;
    vec3 b10 = (4.0*r10 + 2.0*r20) / 6.0;
    vec3 b20 = (2.0*r10 + 4.0*r20) / 6.0;
    vec3 b30 = (r10 + 4.0*r20 + r30) / 6.0;

    vec3 b01 = (r01 + 4.0*r11 + r21) / 6.0;
    vec3 b11 = (4.0*r11 + 2.0*r21) / 6.0;
    vec3 b21 = (2.0*r11 + 4.0*r21) / 6.0;
    vec3 b31 = (r11 + 4.0*r21 + r31) / 6.0;

    vec3 b02 = (r02 + 4.0*r12 + r22) / 6.0;
    vec3 b12 = (4.0*r12 + 2.0*r22) / 6.0;
    vec3 b22 = (2.0*r12 + 4.0*r22) / 6.0;
    vec3 b32 = (r12 + 4.0*r22 + r32) / 6.0;

    vec3 b03 = (r03 + 4.0*r13 + r23) / 6.0;
    vec3 b13 = (4.0*r13 + 2.0*r23) / 6.0;
    vec3 b23 = (2.0*r13 + 4.0*r23) / 6.0;
    vec3 b33 = (r13 + 4.0*r23 + r33) / 6.0;

    // Evaluate Bézier surface

    vec3 q0 = Bezier(u, b00, b01, b02, b03);
    vec3 q1 = Bezier(u, b10, b11, b12, b13);
    vec3 q2 = Bezier(u, b20, b21, b22, b23);
    vec3 q3 = Bezier(u, b30, b31, b32, b33);

    vec3 position = Bezier(v, q0, q1, q2, q3);

    gl_Position =
        projection *
        view *
        scene * 
        model * 
        vec4(position, 1.0);
}