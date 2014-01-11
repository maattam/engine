// GBuffer unpack material fragment shader

#version 420

// R32F        -> Depth attachment
uniform sampler2DMS depthData;

// R10G10B10A2 -> Normal.X, Normal.Y, Shininess, Reserved
uniform sampler2DMS normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Specular intensity
uniform sampler2DMS diffuseSpecData;

// Sample count used when rendering gbuffer
uniform int samples;

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

// Returns the nth sample from a multisampled texture
vec4 sampleTexture(in sampler2DMS sampler, in vec2 uv, int n)
{
    return texelFetch(sampler, ivec2(uv * textureSize(sampler)), n);
}

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

// Returns false if texel is part of skybox
void unpackNormalSpec(inout MaterialInfo material, inout VertexInfo vertex, in vec2 uv, int n)
{
    vec4 data = sampleTexture(normalSpecData, uv, n);

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

void unpackDiffuseSpec(inout MaterialInfo material, in vec2 uv, int n)
{
    vec4 data = sampleTexture(diffuseSpecData, uv, n);

    material.diffuse = data.rgb;
    material.specular = data.a * 255.0;
}

float getSample(inout VertexInfo vertex, inout MaterialInfo material, in vec2 uv, int n)
{
    float z = sampleTexture(depthData, uv, n).r;

    unpackPosition(vertex, z);
    unpackNormalSpec(material, vertex, uv, n);
    unpackDiffuseSpec(material, uv, n);

    return z;
}

bool testEdgeVertex(in vec2 uv)
{
    for(int i = 1; i < samples; ++i)
    {
        if(sampleTexture(normalSpecData, uv, i).r > 0.0)
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

    vec2 uv = calcTexCoord();
    vec4 color = vec4(0.0);

    float z = getSample(vertex, material, uv, 0);
    color += calcAverageOutput(vertex, material, z);

    // Calculate average output for vertex edge samples to perform MSAA
    if(vertex.edge > 0.0 || testEdgeVertex(uv))
    {
        // Average sample
        for(int i = 1; i < samples; ++i)
        {
            z = getSample(vertex, material, uv, i);
            color += calcAverageOutput(vertex, material, z);
        }

        color /= samples;
    }

    if(color == vec4(0))
    {
        discard;
    }

    fragColor = color;
}
