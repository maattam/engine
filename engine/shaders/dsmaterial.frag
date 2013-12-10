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

// Inverse of the projection matrix
uniform mat4 inverseProj;

// Viewport: (left, right, width, height)
uniform vec4 viewport;

// Input quad uv
in vec2 texCoord;

// Output fragment color
layout (location = 0) out vec4 fragColor;

// Unpacked per-fragment data
struct VertexData
{
    vec4 position;
    vec3 normal;
} vertex;

struct MaterialData
{
    vec4 diffuseColor;
    float shininess;
    float specularIntensity;
} material;

// Returns average sample from texture
vec4 sampleTexture(in sampler2DMS sampler, in vec2 uv)
{
    vec4 result = vec4(0, 0, 0, 0);
    ivec2 st = ivec2(viewport.zw * uv);

    for(int i = 0; i < samples; ++i)
    {
        result += texelFetch(sampler, st, i);
    }

    return result / samples;
}

void unpackPosition()
{
    vec4 ndc;
    float depth = sampleTexture(depthData, texCoord);

    ndc.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / viewport.zw - 1;
    ndc.z = (2.0 * depth - depthRange.x - depthRange.y) / (depthRange.y - depthRange.x);
    ndc.w = 1.0;

    vec4 clip = ndc / gl_FragCoord.w;
    vertex.position = inverseProj * clip;
}

void unpackNormalSpec()
{
    vec4 data = sampleTexture(normalSpecData, texCoord);

    // Inverse spheremap transformation
    vec2 fenc = data.rg * 4 - 2;
    float f = dot(fenc, fenc);
    float g = sqrt(1 - f/4);
    vertex.normal.xy = fenc * g;
    vertex.normal.z = 1 - f/2;

    material.specularIntensity = data.b;
}

void unpackDiffuseSpec()
{
    vec4 data = sampleTexture(diffuseSpecData, texCoord);

    material.diffuseColor = vec4(data.rgb, 0);
    material.shininess = data.a;
}

void main()
{
    unpackPosition();
    unpackNormalSpec();
    unpackDiffuseSpec();

    //fragColor = material.diffuseColor;
    //fragColor.rgb = vertex.normal;
    fragColor.rgb = vertex.position.xyz;
}
