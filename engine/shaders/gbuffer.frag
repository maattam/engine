// Basic GBuffer accumulation fragment shader

#version 400

in vec3 texCoord0;
in vec3 normal0;
in vec3 tangent0;

// R10G10B10A2 -> Normal.X, Normal.Y, Specular intensity, Reserved
layout (location = 0) out vec4 normalSpecData;

// R8G8B8A8    -> Diffuse.R, Diffuse.G, Diffuse.B, Shininess
layout (location = 1) out vec4 diffuseSpecData;

struct Material
{
    // Material attributes
	vec3 diffuseColor;
	float shininess;
	float specularIntensity;

    // Material samplers
    sampler2D diffuseSampler;
    sampler2D normalSampler;
    sampler2D specularSampler;
    sampler2D maskSampler;
};

uniform Material material;

subroutine vec3 NormalRoutineType();
subroutine uniform NormalRoutineType calculateNormal;

subroutine(NormalRoutineType)
vec3 calculateBumpedNormal()
{
    vec3 normal = normalize(normal0);
    vec3 tangent = normalize(tangent0);

	// Gramm-Schmidt; We have to form an orthonormal basis since the interpolated
	// vectors are no longer orthogonal
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	
	vec3 bitangent = cross(tangent, normal);
	vec3 bumpMapNormal = texture(material.normalSampler, texCoord0).xyz;

	// Since color components are stored in the range [0, 1], we have to transform
	// them back using f(x) = 2*x - 1
	bumpMapNormal = 2.0 * bumpMapNormal - 1.0;
	
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 newNormal = TBN * bumpMapNormal;
	
	return normalize(newNormal);
}

subroutine(NormalRoutineType)
vec3 calculateInterpolatedNormal()
{
    return normalize(normal0);
}

void packNormalSpecData()
{
    vec3 normal = calculateNormal();

    // Spheremap transform
    // Lambert Azimuthal Equal-Area projection http://aras-p.info/texts/CompactNormalStorage.html
    float p = sqrt(normal.z * 8 + 8);
    normalSpecData.rg = normal.xy / p + 0.5;

    normalSpecData.b = material.specularIntensity * texture2D(material.specularSampler, texCoord0).r;
}

void packDiffuseSpecData()
{
    diffuseSpecData.rgb = texture2D(material.diffuseSampler, texCoord0).rgb * material.diffuseColor;
    diffuseSpecData.a = material.shininess;
}

void main()
{
    // Check if this part of the fragment is opaque according to mask
    if(texture2D(material.maskSampler, texCoord0).r <= 0.2)
        discard;

    packNormalSpecData();
    packDiffuseSpecData();
}