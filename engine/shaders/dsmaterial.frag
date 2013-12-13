// GBuffer unpack material fragment shader

#version 420

// R32F        -> Depth attachment
uniform sampler2DMS depthData;

// R10G10B10A2 -> Normal.X, Normal.Y, Specular intensity, Reserved
uniform sampler2DMS normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Shininess
uniform sampler2DMS diffuseSpecData;

// Sample count used when rendering gbuffer
uniform int samples;

// Near and far values used to produce clip space
uniform vec2 depthRange;

// The perspective projection matrix
uniform mat4 persProj;

uniform vec3 lightDirection;

// Input quad uv
in vec2 texCoord;

// View direction in eye space
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
    vec3 diffuseColor;
    float shininess;
    float specularIntensity;
};

// Subroutine used to implement lightning functions
subroutine vec4 CalculateLightType(in VertexInfo vertex, in MaterialInfo material);
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

	// z/w to ndc z
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

    material.diffuseColor = data.rgb;
    material.shininess = data.a;
}

subroutine(CalculateLightType)
vec4 nullLight(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 lightColor = pow(vec3(1, 1, 251.0 / 255), vec3(2.2)) * 4.5;
    float ambientFac = 0.05;

    vec3 ambient = lightColor * ambientFac;
    float sDotN = max(dot(-lightDirection, vertex.normal), 0.0);
    vec3 diffuse = lightColor * sDotN;

    vec3 specular = vec3(0.0);
    if(sDotN > 0)
    {
        vec3 r = reflect(-lightDirection, vertex.normal);
        float power = pow(max(dot(r, normalize(-vertex.position.xyz)), 0.0), material.shininess);
        specular = lightColor * material.specularIntensity * power;
    }
	
	return vec4(material.diffuseColor * (ambient + diffuse + specular), 0);
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

        color += calculateLight(vertex, material);
    }

    fragColor = color / samples;
}
