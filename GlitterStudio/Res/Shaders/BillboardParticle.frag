#version 330 core

in vec2 uv1;
in vec4 color;

out vec4 fragColor;

struct Material
{
    sampler2D diffuse0;
    sampler2D diffuse1;
    float blend;
};

uniform Material material;
uniform int blendMode;

void main()
{
    vec4 texColor = texture(material.diffuse0, uv1) * color;
    if (blendMode == 5 && texColor.a < 0.5)
        discard;

    fragColor = texColor;
}
