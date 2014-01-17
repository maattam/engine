//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Material shader for debugging gbuffer values
//

#include "dsmaterial.frag"

subroutine(CalculateOutputType)
vec4 outputPositions(in VertexInfo vertex, in MaterialInfo material)
{
    if(vertex.position.z < -1000.0)
    {
        return vec4(vec3(0.01), 1.0);
    }

    return vec4(vertex.position.xy, -vertex.position.z, 0);
}

subroutine(CalculateOutputType)
vec4 outputNormals(in VertexInfo vertex, in MaterialInfo material)
{
    return vec4(vertex.normal, 1.0);
}

subroutine(CalculateOutputType)
vec4 outputDiffuse(in VertexInfo vertex, in MaterialInfo material)
{
    return vec4(material.diffuse, 1.0);
}