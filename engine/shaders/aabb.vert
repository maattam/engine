//
//  Author   : Matti Määttä
//  Type     : Vertex shader
//  Summary  : Shader for drawing wireframe AABBs
//

// AABB vertex shader

#version 420

layout(location = 0) in vec3 position;

uniform mat4 gMVP;

void main()
{
    gl_Position = gMVP * vec4(position, 1.0);
}