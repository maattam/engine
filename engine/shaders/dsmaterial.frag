// GBuffer unpack material fragment shader

#version 420

#define SAMPLES <>

// R32F        -> Depth attachment
uniform sampler2DMS depthData;

// R10G10B10A2 -> Normal.X, Normal.Y, Shininess, Reserved
uniform sampler2DMS normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Specular intensity
uniform sampler2DMS diffuseSpecData;

// Inverse of perspective projection matrix used to accumulate gbuffer
uniform mat4 invPersProj;

// Viewport: x, y, width, height
uniform vec4 viewport;

// Output fragment color
layout (location = 0) out vec4 fragColor;

// Unpacked per-fragment data
struct VertexInfo
{
    vec4 position;
    vec3 normal;
    float edge;
};

struct MaterialInfo
{
    vec3 diffuse;
    float shininess;
    float specular;
};

// Subroutine used to implement different shading functions
subroutine vec4 CalculateOutputType(in VertexInfo vertex, in MaterialInfo material);
subroutine uniform CalculateOutputType calculateOutput;

vec2 calcTexCoord()
{
    return (gl_FragCoord.xy - viewport.xy) / viewport.zw;
}

void unpackPosition(inout VertexInfo vertex, float z)
{
    // Construct ndc position from screen-space coordinates
    vec4 ndcPos;
    ndcPos.xy = (2.0 * gl_FragCoord.xy - 2.0 * viewport.xy) / viewport.zw - 1;
    ndcPos.z = 2.0 * z - 1.0;;
    ndcPos.w = 1.0;

    // Unproject perspective projection
    vec4 viewPos = invPersProj * ndcPos;

    // Get view-space position
    vertex.position = vec4(viewPos.xyz / viewPos.w, 1.0);
}

void unpackNormalSpec(inout MaterialInfo material, inout VertexInfo vertex, in ivec2 st, int n)
{
    vec4 data = texelFetch(normalSpecData, st, n);

    // Inverse spheremap transformation
    vec2 fenc = data.rg * 4 - 2;
    float f = dot(fenc, fenc);
    float g = sqrt(1 - f/4);
    vertex.normal.xy = fenc * g;
    vertex.normal.z = 1 - f/2;

    // Clamp shininess since calculating pow with very small exponent seems to cause problems.
    material.shininess = max(data.b * 1023.0, 0.0001);

    vertex.edge = data.a;
}

void unpackDiffuseSpec(inout MaterialInfo material, in ivec2 st, int n)
{
    vec4 data = texelFetch(diffuseSpecData, st, n);

    material.diffuse = data.rgb;
    material.specular = data.a * 255.0;
}

float getSample(inout VertexInfo vertex, inout MaterialInfo material, in ivec2 st, int n)
{
    float z = texelFetch(depthData, st, n).r;

    unpackPosition(vertex, z);
    unpackNormalSpec(material, vertex, st, n);
    unpackDiffuseSpec(material, st, n);

    return z;
}

bool testEdgeVertex(in ivec2 st)
{
    for(int i = 1; i < SAMPLES; ++i)
    {
        if(texelFetch(normalSpecData, st, i).a > 0.0)
        {
            return true;
        }
    }

    return false;
}

vec4 calcAverageOutput(in VertexInfo vertex, in MaterialInfo material, float z)
{
    if(gl_FragCoord.z > z)
    {
        return vec4(0.0);
    }

    return calculateOutput(vertex, material);
}

void main()
{
    VertexInfo vertex;
    MaterialInfo material;

    ivec2 st = ivec2(calcTexCoord() * textureSize(depthData));
    vec4 color = vec4(0.0);

    float z = getSample(vertex, material, st, 0);
    color += calcAverageOutput(vertex, material, z);

    // Calculate average output for vertex edge samples to resolve MSAA
    if(vertex.edge > 0.0 || testEdgeVertex(st))
    {
        // Average sample
        for(int i = 1; i < SAMPLES; ++i)
        {
            z = getSample(vertex, material, st, i);
            color += calcAverageOutput(vertex, material, z);
        }

        color /= SAMPLES;
    }

    if(color.rgb == vec3(0))
    {
        discard;
    }

    fragColor = color;
}
