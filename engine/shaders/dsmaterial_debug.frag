// Material shader for debugging gbuffer values

#include "dsmaterial.frag"

subroutine(CalculateLightType)
vec4 outputPositions(in VertexInfo vertex, in MaterialInfo material, int n)
{
    return vec4(vertex.position.xyz / vec3(15), 0);
}

subroutine(CalculateLightType)
vec4 outputNormals(in VertexInfo vertex, in MaterialInfo material, int n)
{
    return vec4(vertex.normal, 0);
}

subroutine(CalculateLightType)
vec4 outputDiffuse(in VertexInfo vertex, in MaterialInfo material, int n)
{
    return material.diffuseColor;
}