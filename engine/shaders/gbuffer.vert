//
//  Author   : Matti Määttä
//  Type     : Vertex shader
//  Summary  : GBuffer accumulation shader
//

#version 420

#define SAMPLES <>

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

out vec2 texCoord0;
out vec3 normal0;
out mat3 TBN;

out vec2 maybeOutside;
centroid out vec2 certainlyOutside;

uniform mat4 MVP;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

subroutine void TangentPassType();
subroutine uniform TangentPassType tangentPass;

subroutine(TangentPassType)
void skipTangent()
{
}

subroutine(TangentPassType)
void calculateTangent()
{
    vec3 normal = normalize(normal0);
    vec3 tangent = normalize(normalMatrix * vertexTangent);
    vec3 bitangent = cross(tangent, normal);
    TBN = mat3(tangent, bitangent, normal);
}

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    normal0 = normalMatrix * vertexNormal;
    texCoord0 = vertexTexCoord;

#if SAMPLES > 1
    // Use centroid sampling to perform edge detection
    maybeOutside = gl_Position.xy;
    certainlyOutside = gl_Position.xy;
#endif

    tangentPass();
}