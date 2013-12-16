// GBuffer unpack vertex shader

#version 420

layout(location = 0) in vec3 vertexPosition;

out vec2 texCoord;
out vec3 eyeDirection;

void main()
{
    gl_Position = vec4(vertexPosition, 1.0);
    texCoord = (vertexPosition.xy + vec2(1.0, 1.0)) / 2.0;
    eyeDirection = vec3(vertexPosition.xy, -1);
}