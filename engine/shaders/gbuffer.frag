//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : GBuffer accumulation shader
//

#version 420

in vec2 texCoord0;
in vec3 normal0;
in mat3 TBN;

in vec2 maybeOutside;
centroid in vec2 certainlyOutside;

// R10G10B10A2 -> Normal.X, Normal.Y, Shininess, Reserved
layout (location = 0) out vec4 normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Specular intensity
layout (location = 1) out vec4 diffuseSpecData;

struct Material
{
    // Material attributes
    vec3 diffuse;
    float shininess;
    float specular;

    // Material samplers
    sampler2D diffuseSampler;
    sampler2D normalSampler;
    sampler2D specularSampler;
    sampler2D maskSampler;
    sampler2D shininessSampler;
};

uniform Material material;

subroutine vec3 NormalRoutineType();
subroutine uniform NormalRoutineType calculateNormal;

subroutine(NormalRoutineType)
vec3 calculateInterpolatedNormal()
{
    return normalize(normal0);
}

subroutine(NormalRoutineType)
vec3 calculateBumpedNormal()
{
    // Since color components are stored in the range [0, 1], we have to transform
    // them back using f(x) = 2*x - 1
    vec3 bumpMapNormal = texture(material.normalSampler, texCoord0).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - 1.0;

    vec3 newNormal = TBN * bumpMapNormal;
    return normalize(newNormal);
}

void packNormalSpecData()
{
    vec3 normal = calculateNormal();

    // Spheremap transform
    // Lambert Azimuthal Equal-Area projection http://aras-p.info/texts/CompactNormalStorage.html
    float p = sqrt(normal.z * 8 + 8);
    normalSpecData.rg = normal.xy / p + 0.5;

    normalSpecData.b = texture(material.shininessSampler, texCoord0).r * material.shininess / 1023.0;

    // Write potential vertex edges to output.
    if(any(maybeOutside != certainlyOutside))
    {
        normalSpecData.a = 1.0;
    }
}

void packDiffuseSpecData()
{
    diffuseSpecData.rgb = texture(material.diffuseSampler, texCoord0).rgb * material.diffuse;
    diffuseSpecData.a = texture(material.specularSampler, texCoord0).r * material.specular / 255.0;
}

void main()
{
    // Check if this part of the fragment is opaque according to mask
    if(texture(material.maskSampler, texCoord0).r <= 0.2)
        discard;

    packNormalSpecData();
    packDiffuseSpecData();
}