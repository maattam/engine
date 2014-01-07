// GBuffer unpack vertex shader

#version 420

layout(location = 0) in vec3 vertexPosition;

uniform float quadScale;
uniform vec3 quadCenter;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform mat4 viewProj;

// Subroutine used to change between fullscreen and screen-oriented quads.
subroutine void TransformQuadType();
subroutine uniform TransformQuadType transformQuad;

subroutine(TransformQuadType)
void screenOrientedQuad()
{
    vec3 position = quadCenter
            + cameraRight * vertexPosition.x * quadScale
            + cameraUp * vertexPosition.y * quadScale;

    gl_Position = viewProj * vec4(position, 1);
}

subroutine(TransformQuadType)
void fullscreenQuad()
{
    gl_Position = vec4(vertexPosition, 1.0);
}

void main()
{
    transformQuad();
}