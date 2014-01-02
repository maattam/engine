// Material shader for debugging gbuffer values

#include "dsmaterial.frag"

subroutine(CalculateOutputType)
vec4 outputPositions(in VertexInfo vertex, in MaterialInfo material)
{
    return vec4(vertex.position.xyz / 15.0, 0);
}

subroutine(CalculateOutputType)
vec4 outputNormals(in VertexInfo vertex, in MaterialInfo material)
{
    return vec4(vertex.normal, 0);
}

subroutine(CalculateOutputType)
vec4 outputDiffuse(in VertexInfo vertex, in MaterialInfo material)
{
    return vec4(material.diffuseColor, 0);
}