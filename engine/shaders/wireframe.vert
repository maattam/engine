//
//  Author   : Matti Määttä
//  Type     : Vertex shader
//  Summary  : Wireframe debugging shader.
//

#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

uniform mat4 gMVP;

out vec2 texCoord0;

void main()
{
    gl_Position = gMVP * vec4(position, 1.0);

    texCoord0 = texCoord;
}