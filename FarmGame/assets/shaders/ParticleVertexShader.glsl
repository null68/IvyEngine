#version 330 core

layout(location = 0) in vec2 a_QuadPosition; 
layout(location = 1) in vec2 a_QuadUV;

layout(location = 2) in vec4 a_PositionSize;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in float a_Rotation;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_UV;
out vec4 v_Color;

void main()
{

    vec3 cameraRight = vec3(u_View[0][0], u_View[1][0], u_View[2][0]);
    vec3 cameraUp    = vec3(u_View[0][1], u_View[1][1], u_View[2][1]);

    float c = cos(a_Rotation);
    float s = sin(a_Rotation);
    vec2 rotated = vec2(
        a_QuadPosition.x * c - a_QuadPosition.y * s,
        a_QuadPosition.x * s + a_QuadPosition.y * c
    );

    vec3 worldPos = a_PositionSize.xyz
        + (rotated.x * cameraRight + rotated.y * cameraUp) * a_PositionSize.w;

    gl_Position = u_Projection * u_View * vec4(worldPos, 1.0);

    v_UV = a_QuadUV;
    v_Color = a_Color;
}
