//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Deferred shading material shader for implementing different lightning models.
//

#include "dsmaterial.frag"

struct Light
{
    // Color is expected to be linear
    vec3 color;

    // Direction and position are expected to be in eye -coordinates
    vec3 direction;
    vec3 position;

    // Directional light only, applied on top of shadow factor
    float ambientIntensity;

    // Spot light only, cosine of cutoff angle
    float cosOuterAngle;
    float cosInnerAngle;

    // Attenuation vector: (constant, linear, quadratic)
    vec3 attenuation;
};

uniform Light light;

uniform mat4 lightVP;
uniform mat4 viewInverse;
uniform vec2 shadowOffset;
uniform sampler2DShadow shadowSampler;

#define SHADOW_BIAS 0.0002

vec3 lightningModel(in vec3 lightToFragment, in VertexInfo vertex, in MaterialInfo material)
{
    vec3 n = vertex.normal;
    vec3 l = normalize(lightToFragment);

    // Lambertian reflectance
    float lambert = max(dot(l, n), 0.0);

    vec3 diffuse = lambert * light.color;
    vec3 specular = vec3(0.0);

    if(lambert > 0.0)
    {
        // Source: Crafting Physically Motivated Shading Models for Game Developement, by Naty Hoffmann

        // Compute halfway vector for Blinn-Phong model
        vec3 v = normalize(-vertex.position.xyz);
        vec3 h = normalize(v + l);

        // Cosine power normalisation factor
        float d = (material.shininess + 2) / 8;
        float power = pow(max(dot(h, n), 0.0), material.shininess);
        
        // Calculate Schlick fresnel approximation to weigth interpolation between texel specular intensity
        // and full specular hilight.
        float lh = dot(l, h);
        float fschlick = min(pow(1 - max(lh, 0.0), 5), 1.0);

        // Approximation of the Cook-Torrance geometry factor. For science.
        float G = 1.0 / max(lh * lh, 0.01);

        specular = G * d * power * mix(material.specular, 1.0, fschlick) * light.color;
    }

    return diffuse + specular;
}

float softShadowModel(in vec4 lightSpacePos, in sampler2DShadow shadowMap, in vec2 offset)
{
    // Project shadow map on current fragment
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = 0.5 * projCoords + 0.5;

    float factor = 0.0;

    // PCF kernel
    for(int y = -1; y <= 1; ++y)
    {
        for(int x = -1; x <= 1; ++x)
        {
            vec2 offsets = vec2(x, y) * offset;
            vec3 uvc = projCoords + vec3(offsets, 0);

            factor += texture(shadowMap, uvc, SHADOW_BIAS);
        }
    }

    return factor / 9.0;
}

subroutine(CalculateOutputType)
vec4 pointLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    // Light ray to fragment
    vec3 lightToFragment = light.position - vertex.position.xyz;
    float dist = length(lightToFragment);

    vec3 color = lightningModel(lightToFragment, vertex, material);

    // Point light attenuation
    float attenuation = light.attenuation.x +
                        light.attenuation.y * dist +
                        light.attenuation.z * dist * dist;

    return vec4(material.diffuse * color / attenuation, 1.0);
}

subroutine(CalculateOutputType)
vec4 spotLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 lightToFragment = normalize(light.position - vertex.position.xyz);

    float cosAngle = dot(-lightToFragment, light.direction);
    float angleDiff = light.cosInnerAngle - light.cosOuterAngle;

    float spotFactor = clamp((cosAngle - light.cosOuterAngle) / angleDiff, 0.0, 1.0);

    vec3 color = pointLightPass(vertex, material).rgb * spotFactor;
    return vec4(material.diffuse * color, 1.0);
}

subroutine(CalculateOutputType)
vec4 spotLightPassShadow(in VertexInfo vertex, in MaterialInfo material)
{
	// Transform fragment position to light space
    vec4 lightSpacePos = lightVP * (viewInverse * vertex.position);
    float shadow = softShadowModel(lightSpacePos, shadowSampler, shadowOffset);

	return vec4(spotLightPass(vertex, material).rgb * shadow, 1.0);
}

subroutine(CalculateOutputType)
vec4 directionalLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 ambient = light.color * light.ambientIntensity;
    vec3 color = lightningModel(-light.direction, vertex, material) + ambient;

    return vec4(material.diffuse * color, 1.0);
}