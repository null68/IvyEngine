#version 330 core

in vec2 v_UV;
in vec4 v_Color;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform bool u_UseTexture;

void main()
{
    vec4 baseColor = u_UseTexture ? texture(u_Texture, v_UV) : vec4(1.0);
    vec4 result = baseColor * v_Color;

    if (result.a < 0.01)
        discard;

    FragColor = result;
}
