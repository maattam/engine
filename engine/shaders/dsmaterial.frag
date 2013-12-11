// GBuffer unpack material fragment shader

#version 400

// R32F        -> Depth attachment
uniform sampler2DMS depthData;

// R10G10B10A2 -> Normal.X, Normal.Y, Specular intensity, Reserved
uniform sampler2DMS normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Shininess
uniform sampler2DMS diffuseSpecData;

// Sample count used when rendering gbuffer
uniform int samples;

// Near and far values used to produce the clip space
uniform vec2 depthRange;

// The perspective projection matrix
uniform mat4 persProj;

// Input quad uv
in vec2 texCoord;

in vec3 eyeDirection;

// Output fragment color
layout (location = 0) out vec4 fragColor;

// Unpacked per-fragment data
struct VertexInfo
{
    vec4 position;
    vec3 normal;
};

struct MaterialInfo
{
    vec4 diffuseColor;
    float shininess;
    float specularIntensity;
};

// Subroutine used to implement lightning functions
// n is the sample index.
subroutine vec4 CalculateLightType(in VertexInfo vertex, in MaterialInfo material, int n);
subroutine uniform CalculateLightType calculateLight;

// Returns the nth sample from a multisampled texture
vec4 sampleTexture(in sampler2DMS sampler, in vec2 uv, int n)
{
    ivec2 st = ivec2(textureSize(sampler) * uv);
    return texelFetch(sampler, st, n);
}

void unpackPosition(inout VertexInfo vertex, int n)
{
    float z = sampleTexture(depthData, texCoord, n).z;

    float ndcZ = (2.0 * z - depthRange.x - depthRange.y) / (depthRange.y - depthRange.x);
    float eyeZ = persProj[3][2] / ((persProj[2][3] * ndcZ) - persProj[2][2]);
    vertex.position = vec4(eyeDirection * eyeZ, 1);
}

void unpackNormalSpec(inout MaterialInfo material, inout VertexInfo vertex, int n)
{
    vec4 data = sampleTexture(normalSpecData, texCoord, n);

    // Inverse spheremap transformation
    vec2 fenc = data.rg * 4 - 2;
    float f = dot(fenc, fenc);
    float g = sqrt(1 - f/4);
    vertex.normal.xy = fenc * g;
    vertex.normal.z = 1 - f/2;

    material.specularIntensity = data.b;
}

void unpackDiffuseSpec(inout MaterialInfo material, int n)
{
    vec4 data = sampleTexture(diffuseSpecData, texCoord, n);

    material.diffuseColor = vec4(data.rgb, 0);
    material.shininess = data.a;
}

subroutine(CalculateLightType)
vec4 nullLight(in VertexInfo vertex, in MaterialInfo material, int n)
{
    discard;
    return vec4(0, 0, 0, 0);
}

void main()
{
    VertexInfo vertex;
    MaterialInfo material;

    vec4 color = vec4(0, 0, 0, 0);

    for(int i = 0; i < samples; ++i)
    {
        unpackPosition(vertex, i);
        unpackNormalSpec(material, vertex, i);
        unpackDiffuseSpec(material, i);

        color += calculateLight(vertex, material, i);
    }

    fragColor = color / samples;
}
