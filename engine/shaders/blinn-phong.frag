#include "dsillumination.frag"

subroutine(LightningModelType)
vec3 blinnPhongModel(in vec3 lightDirection, in VertexInfo vertex, in MaterialInfo material)
{
    float sDotN = max(dot(-lightDirection, vertex.normal), 0.0);
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