#version 330 core

in vec2 uv;
in vec4 vColor;
out vec4 fragColor;

struct Material
{
    sampler2D diffuse0;
    sampler2D diffuse1;
    float blend;
};

uniform Material material;
uniform vec4 color;
uniform vec2 uvOffset;
uniform int blendMode;

void main()
{
    vec4 result = texture(material.diffuse0, uv + uvOffset) * color * vColor;
    if (blendMode == 5 && result.a < 0.5)
        discard;

    fragColor = result;
}
