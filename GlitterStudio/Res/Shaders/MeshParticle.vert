#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;

out vec2 uv;
out vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    uv          = aUV;
    vColor      = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}