// Basic lightning fragment shader

#version 330 core

const int MAX_POINT_LIGHTS = 4;
const int MAX_SPOT_LIGHTS = 4;

in vec2 texCoord0;
in vec3 normal0;
in vec3 tangent0;
in vec3 worldPos0;

in vec4 lightSpacePos0[MAX_SPOT_LIGHTS];

layout(location = 0) out vec4 fragColor;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct Attenuation
{
	float constant;
	float linear;
	float exp;
};

struct PointLight
{
	Light base;
	Attenuation attenuation;
	vec3 position;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float cutoff;
};

struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
	float shininess;
	float specularIntensity;
};

uniform int gNumPointLights;
uniform int gNumSpotLights;

uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];

uniform sampler2D gDiffuseSampler;
uniform sampler2D gNormalSampler;
uniform sampler2D gSpecularSampler;

uniform sampler2D gSpotLightShadowMap[MAX_SPOT_LIGHTS];

uniform Material gMaterial;
uniform vec3 gEyeWorldPos;
uniform bool gHasTangents;

float calcShadowFactor(in vec4 lightSpacePos, in sampler2D shadowMap)
{

    const vec2 poissonDisk[4] = {
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760)
    };

    float bias = 0.0002;

	// Project shadow map on current fragment
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = 0.5 * projCoords + 0.5;
	
	float visibility = 1.0f;
    
    for(int i = 0; i < 4; ++i)
    {
        if(texture(shadowMap, projCoords.xy + poissonDisk[i]/700.0).z < projCoords.z - bias)
        {
            visibility -= 1.0 / 3.0;
        }
    }

    if(visibility < 0.0)
        visibility = 0.0;

    return visibility;
}

// Encapsules common stuff between the different light types
vec4 calcLightCommon(in Light light, in vec3 lightDirection, in vec3 normal)
{
	vec4 ambientColor = vec4(light.color, 1.0) * light.ambientIntensity;
	
	// Diffuse factor depends on the (positive) cosine between surface normal and light direction
	float diffuseFactor = clamp(dot(normal, -lightDirection), 0, 1);
	
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specularColor = vec4(0, 0, 0, 0);
	
	if(diffuseFactor > 0)
	{
		diffuseColor = vec4(light.color, 1.0) * light.diffuseIntensity * diffuseFactor;
	
		vec3 vertexToEye = normalize(gEyeWorldPos - worldPos0);
		vec3 lightReflect = normalize(reflect(lightDirection, normal));
		
		// Specular reflection is almost the same as diffuse, but here we compare against the eye direction
		float specularFactor = clamp(dot(vertexToEye, lightReflect), 0, 1);
		specularFactor = pow(specularFactor, gMaterial.shininess);
		
		if(specularFactor > 0)
		{
			specularColor = vec4(light.color, 1.0) * texture2D(gSpecularSampler, texCoord0) *
				gMaterial.specularIntensity * specularFactor;
		}
	}
	
	return ambientColor + diffuseColor + specularColor;
}

vec4 calcDirectionalLight(in vec3 normal)
{
	return calcLightCommon(gDirectionalLight.base, gDirectionalLight.direction, normal);
}

vec4 calcPointLight(in PointLight light, in vec3 normal)
{
	vec3 lightDirection = worldPos0 - light.position;
	float dist = length(lightDirection);
	lightDirection = normalize(lightDirection);
	
	vec4 color = calcLightCommon(light.base, lightDirection, normal);
	float attenuation = light.attenuation.constant +
						light.attenuation.linear * dist +
						light.attenuation.exp * dist * dist;
						
	return color / attenuation;
}

vec4 calcSpotLight(in SpotLight light, in vec3 normal)
{
	vec3 lightToPixel = normalize(worldPos0 - light.base.position);
	float spotFactor = dot(lightToPixel, light.direction);
	
	if(spotFactor > light.cutoff)
	{
		vec4 color = calcPointLight(light.base, normal);
		return color * (1.0 - (1.0 - spotFactor) * 1.0 / (1.0 - light.cutoff));
	}
	
	else
	{
		return vec4(0, 0, 0, 0);
	}
}

vec3 calcBumpedNormal()
{
    // We have to normalize our normal (again) since the fragment shader does interpolation between vertices
	vec3 normal = normalize(normal0);
	
	if(!gHasTangents)
		return normal;
		
	vec3 tangent = normalize(tangent0);

	// Gramm-Schmidt; We have to form an orthonormal basis since the interpolated
	// vectors are no longer orthogonal
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	
	vec3 bitangent = cross(tangent, normal);
	vec3 bumpMapNormal = texture(gNormalSampler, texCoord0).xyz;

	// Since color components are stored in the range [0, 1], we have to transform
	// them back using f(x) = 2*x - 1
	bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
	
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 newNormal = TBN * bumpMapNormal;
	
	return normalize(newNormal);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	vec3 normal = calcBumpedNormal();
	vec4 light = calcDirectionalLight(normal);
	
	for(int i = 0; i < gNumPointLights; ++i)
    {
		light += calcPointLight(gPointLights[i], normal);
    }

	for(int i = 0; i < gNumSpotLights; ++i)
	{
		float shadow = calcShadowFactor(lightSpacePos0[i], gSpotLightShadowMap[i]);
		light += shadow * calcSpotLight(gSpotLights[i], normal);
	}
	
	fragColor = texture2D(gDiffuseSampler, texCoord0) * light *
				vec4(gMaterial.diffuseColor, 1.0) + vec4(gMaterial.ambientColor, 1.0);

    // Oversaturate by 20%
    vec3 fraghsv = rgb2hsv(fragColor.rgb);
    fraghsv.g = clamp(fraghsv.g * 1.2, 0, 1);
    fragColor = vec4(hsv2rgb(fraghsv), 1.0);
}