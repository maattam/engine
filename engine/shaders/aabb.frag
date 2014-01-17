//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Shader for drawing wireframe AABBs
//

// AABB fragment shader

#version 420

layout(location = 0) out vec4 fragColor;

uniform vec3 gColor;

void main()
{
    fragColor = vec4(gColor, 1.0);
}