// Basic GBuffer accumulation vertex shader

#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

out vec3 texCoord0;
out vec3 normal0;
out vec3 tangent0;

uniform mat4 MVP;
uniform mat4 modelViewMatrix;

subroutine vec3 TangentPassType();
subroutine uniform TangentPassType tangentPass;

subroutine(TangentPassType)
void calculateTangent()
{
    tangent0 = (modelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
}

subroutine(TangentPassType)
void skipTangent()
{
}

void main()
{
    normal0 = (modelViewMatrix * vec4(vertexNormal, 0.0)).xyz;
    texCoord0 = vertexTexCoord;
    tangentPass();

    gl_Position = MVP * vec4(vertexPosition, 1.0);
}