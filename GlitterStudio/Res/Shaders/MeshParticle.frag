#version 330 core

in vec2 uv;
out vec4 fragColor;

struct Material
{
    sampler2D diffuse0;
    sampler2D diffuse1;
    float blend;
};

uniform Material material;
uniform vec4 color;

void main()
{
    fragColor = texture(material.diffuse0, uv) * color;
}
