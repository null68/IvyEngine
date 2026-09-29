#version 330 core

layout(location = 0) in vec2 a_QuadPosition;
layout(location = 1) in vec2 a_QuadUV;

layout(location = 2) in vec2 a_RectPosition;
layout(location = 3) in vec2 a_RectSize;   
layout(location = 4) in vec4 a_Color;
layout(location = 5) in vec4 a_UVRect;      
layout(location = 6) in float a_Rotation; 

uniform mat4 u_View; 
uniform mat4 u_Projection;

out vec2 v_UV;
out vec4 v_Color;

void main()
{
    float rad = radians(a_Rotation);
    float c = cos(rad);
    float s = sin(rad);
    vec2 rotated = vec2(
        a_QuadPosition.x * c - a_QuadPosition.y * s,
        a_QuadPosition.x * s + a_QuadPosition.y * c
    );


    vec2 pixelPos = a_RectPosition + a_RectSize * 0.5 + rotated * a_RectSize;

    gl_Position = u_Projection * u_View * vec4(pixelPos, 0.0, 1.0);

    v_UV = mix(a_UVRect.xy, a_UVRect.zw, a_QuadUV);
    v_Color = a_Color;
}
