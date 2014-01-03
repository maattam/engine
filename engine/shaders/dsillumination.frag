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

vec3 lightningModel(in vec3 lightDirection, in VertexInfo vertex, in MaterialInfo material)
{
    float sDotN = max(dot(lightDirection, vertex.normal), 0.0);
    vec3 diffuse = light.color * sDotN;

    vec3 specular = vec3(0.0);
    if(sDotN > 0)
    {
        vec3 r = reflect(-lightDirection, vertex.normal);
        float power = pow(max(dot(r, normalize(-vertex.position.xyz)), 0.0), material.shininess);
        specular = light.color * material.specularIntensity * power;
    }
	
	return diffuse + specular;
}

subroutine(CalculateOutputType)
vec4 pointLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    // Light ray to fragment
    vec3 lightToFragment = light.position - vertex.position.xyz;
    float dist = length(lightToFragment);

    vec3 color = lightningModel(normalize(lightToFragment), vertex, material);

    // Spot light attenuation
    float attenuation = light.attenuation.constant +
                        light.attenuation.linear * dist +
                        light.attenuation.quadratic * dist * dist;

    return vec4(material.diffuseColor * color / attenuation, 1.0);
}

subroutine(CalculateOutputType)
vec4 spotLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 lightToFragment = normalize(light.position - vertex.position.xyz);
    float spotFactor = dot(lightToFragment, -light.direction);
    vec4 color = vec4(0, 0, 0, 1.0);

    // TODO: Interpolate between inner and outer cutoff
    if(spotFactor > light.outerAngle)
    {
        color = pointLightPass(vertex, material);
        color = color * (1.0 - (1.0 - spotFactor) * 1.0 / (1.0 - light.outerAngle));
    }

    return vec4(material.diffuseColor, 1.0) * color;
}

subroutine(CalculateOutputType)
vec4 directionalLightPass(in VertexInfo vertex, in MaterialInfo material)
{
    vec3 ambient = light.color * light.ambientIntensity;
    vec3 color = lightningModel(light.direction, vertex, material) + ambient;

    return vec4(material.diffuseColor * color, 1.0);
}