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

void unpackPosition(inout VertexInfo vertex, int n)
{
    float z = sampleTexture(depthData, calcTexCoord(), n).r;

    if(gl_FragCoord.z > z)
    {
        discard;
    }

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
void unpackNormalSpec(inout MaterialInfo material, inout VertexInfo vertex, int n)
{
    vec4 data = sampleTexture(normalSpecData, calcTexCoord(), n);

    // Inverse spheremap transformation
    vec2 fenc = data.rg * 4 - 2;
    float f = dot(fenc, fenc);
    float g = sqrt(1 - f/4);
    vertex.normal.xy = fenc * g;
    vertex.normal.z = 1 - f/2;

    // Clamp shininess since calculating pow with very small exponent seems to cause problems.
    material.shininess = max(data.b * 1023.0, 0.0001);
}

void unpackDiffuseSpec(inout MaterialInfo material, int n)
{
    vec4 data = sampleTexture(diffuseSpecData, calcTexCoord(), n);

    material.diffuse = data.rgb;
    material.specular = data.a * 255.0;
}

void main()
{
    VertexInfo vertex;
    MaterialInfo material;

    vec4 color = vec4(0);

    for(int i = 0; i < samples; ++i)
    {
        unpackPosition(vertex, i);
        unpackNormalSpec(material, vertex, i);
        unpackDiffuseSpec(material, i);

        color += calculateOutput(vertex, material);
    }

    fragColor = color / samples;
}
