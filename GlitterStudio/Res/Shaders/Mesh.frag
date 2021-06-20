#version 330 core

in vec3 fragPos;
in vec3 fragNrm;
in vec2 uv0;
in vec2 uv1;
in vec2 uv2;
in vec2 uv3;
in vec4 vColor;
in mat3 tbn;
out vec4 fragColor;

struct Material
{
    sampler2D diffuse0;
    sampler2D gloss0;
    sampler2D normal0;
    sampler2D emissive0;
    sampler2D specular0;
    sampler2D displacement0;
    sampler2D reflection0;
    int diffIndex;
    int glossIndex;
    int normIndex;
    int emsIndex;
    int disIndex;
    int specIndex;
    int dispIndex;
    int refIndex;
    vec2 uvOffset0;
    vec2 uvOffset1;
    vec2 uvOffset2;
    vec2 uvOffset3;
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 power_gloss_level;
    bool hasNormal;
    bool hasGloss;
};

struct Light
{
    vec3 position;
    vec3 color;
    float ambient;
    float specular;
    int type; // { -1: "disabled", 0: "direct", 1: "point", 2: "spotlight" }
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform int blendMode;

vec2 getUVIndex(int index)
{
    switch (index)
    {
    case 0:
        return uv0;
    case 1:
        return uv1;
    case 2:
        return uv2;
    case 3:
        return uv3;
    default:
        return uv0;
    }
}

vec2 getUVOffset(int index)
{
    switch (index)
    {
    case 0:
        return material.uvOffset0;

    case 1:
        return material.uvOffset1;

    case 2:
        return material.uvOffset2;

    case 3:
        return material.uvOffset3;

    default:
        return material.uvOffset0;
    }
}

vec4 calcDiffuse()
{
    // ambient
    vec2 diffUV = getUVIndex(material.diffIndex);
    vec2 diffUVOffset = getUVOffset(material.diffIndex);
    vec4 ambient = vec4(light.ambient * light.color * material.ambient.rgb, 1.0) * texture(material.diffuse0, diffUV + diffUVOffset);

    // normal
    vec3 normal = normalize(fragNrm);
    if (material.hasNormal)
    {
        vec2 nrmUV = getUVIndex(material.normIndex);
        vec2 nrmUVOffset = getUVOffset(material.normIndex);
        vec4 normalMap = texture(material.normal0, nrmUV + nrmUVOffset);
        normalMap = normalMap * 2.0 - 1.0;
        normalMap.x *= normalMap.w;
        normalMap.z = sqrt(1 - dot(normalMap.xy, normalMap.xy));

        normal = normalize(tbn * normalMap.xyz);
    }

    vec2 emsUV = getUVIndex(material.emsIndex);
    vec2 emsUVOffset = getUVOffset(material.emsIndex);
    vec4 emissive = material.emissive * texture(material.emissive0, emsUV + emsUVOffset);

    // diffuse
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(lightDir, normal), 0.0);
    vec4 diffuse = diff * vec4(light.color * material.diffuse.rgb, 1.0) * texture(material.diffuse0, diffUV + diffUVOffset);

    // gloss
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = 0.0;
    if (material.hasGloss)
    {
        vec2 glossUV = getUVIndex(material.glossIndex);
        vec2 glossUVOffset = getUVIndex(material.glossIndex);
        float specularPower = max(1, min(1024, (texture(material.gloss0, glossUV + diffUVOffset) * material.power_gloss_level).y * 500));
        float specBrightness = (texture(material.gloss0, glossUV + diffUVOffset) * material.power_gloss_level).z * 5;
        
        spec =  pow(max(dot(halfwayDir, normal), 0.0), specularPower) * specBrightness;
    }

    vec4 specular = vec4(light.color * light.specular, 1.0) * spec;

    // reflection
    //vec3 r = reflect(viewDir, normalize(normal));
    //vec4 reflec = texture(material.reflection0, uv0) * spcMap.a;

    return vec4(ambient + diffuse + specular + emissive);
}

void main()
{
    vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
    if (light.type != -1)
        result = calcDiffuse();
    else
    {
        vec2 diffUV = getUVIndex(material.diffIndex);
        result = texture(material.diffuse0, diffUV);
    }

    if (blendMode == 5 && result.a < 0.5)
        discard;

    fragColor = result;
}
