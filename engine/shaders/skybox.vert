#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 gMVP;

out vec3 texCoord0;

void main()
{
    vec4 MVP_pos = gMVP * vec4(position, 1.0);
    gl_Position = MVP_pos.xyww;
    texCoord0 = normalize(position);
}