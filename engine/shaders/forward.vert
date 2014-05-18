//
//  Author   : Matti Määttä
//  Type     : Vertex shader
//  Summary  : Forward stage vertex shader
//

#version 420

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 MVP;

out vec2 texCoord0;
out vec3 worldPos0;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    
    texCoord0 = vertexTexCoord;
}