#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNrm;
layout (location = 2) in vec3 aTan;
layout (location = 3) in vec3 aBiN;
layout (location = 4) in vec4 aColor;
layout (location = 5) in vec2 aUV0;
layout (location = 6) in vec2 aUV1;
layout (location = 7) in vec2 aUV2;
layout (location = 8) in vec2 aUV3;

out vec3 fragPos;
out vec3 fragNrm;
out vec2 uv0;
out vec2 uv1;
out vec2 uv2;
out vec2 uv3;
out vec4 vColor;
out mat3 tbn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

mat3 createTBN()
{
    vec3 t = normalize(vec3(model * vec4(aTan, 0.0)));
    vec3 n = normalize(vec3(model * vec4(aNrm, 0.0)));

    t = normalize(t - dot(t, t) * n);
    vec3 b = cross(n, t);

    return mat3(t, b, n);
}

void main()
{
    tbn = createTBN();
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNrm = mat3(transpose(inverse(model))) * aNrm;

    uv0         = aUV0;
    uv1         = aUV1;
    uv2         = aUV2;
    uv3         = aUV3;
    vColor      = aColor;
    gl_Position = projection * view * vec4(fragPos, 1.0);
}