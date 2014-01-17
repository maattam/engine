//
//  Author   : Matti Määttä
//  Type     : Vertex shader
//  Summary  : GBuffer unpack material shader.
//             Fills the screen with a screen-aligned quad.
//

#version 420

layout(location = 0) in vec3 vertexPosition;

void main()
{
    gl_Position = vec4(vertexPosition, 1.0);
}