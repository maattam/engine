// Abstract illumination material shader for implementing different lightning models.

#include "dsmaterial.frag"

struct Attenuation
{
	float constant;
	float linear;
	float quadratic;
};

struct Light
{
    // Color is expected to be linear
	vec3 color;

    // Direction and position are expected to be in eye -coordinates
    vec3 direction;
    vec3 position;

    // Directional light only, applied on top of shadow factor
    float ambientIntensity;

    // Spot light only, angle is in radians
	float outerAngle;
    float innerAngle;

    Attenuation attenuation;
};

uniform Light light;

// Subroutine used to implement different lightning response models.
subroutine vec3 LightningModelType(in vec3 lightDirection, in VertexInfo vertex, in MaterialInfo material);
subroutine uniform LightningModelType lightningModel;

subroutine vec3 LightPassType(in VertexInfo vertex, in MaterialInfo material);
subroutine uniform LightPassType lightPass;

subroutine(LightPassType)
vec3 pointLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    // Light ray to fragment
    vec3 lightToFragment = vertex.position.xyz - light.position;
    float dist = length(lightToFragment);

    vec3 color = lightningModel(normalize(lightToFragment), vertex, material);

    // Spot light attenuation
    float attenuation = light.attenuation.constant +
                        light.attenuation.linear * dist +
                        light.attenuation.quadratic * dist * dist;

    return color / attenuation;
}

subroutine(LightPassType)
vec3 spotLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 lightToFragment = normalize(vertex.position.xyz - light.position);
    float spotFactor = dot(lightToFragment, light.direction);

    // TODO: Interpolate between inner and outer cutoff
    if(spotFactor > light.outerAngle)
    {
        vec3 color = pointLightPass(vertex, material);
        return color * (1.0 - (1.0 - spotFactor) * 1.0 / (1.0 - light.outerAngle));
    }

    return vec3(0);
}

subroutine(LightPassType)
vec3 directionalLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 ambient = light.color * light.ambientIntensity;
    return lightningModel(light.direction, vertex, material) + ambient;
}

subroutine(CalculateOutputType)
vec4 illumination(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 lightColor = lightPass(vertex, material);
    return vec4(material.diffuseColor * lightColor, 1.0);
}