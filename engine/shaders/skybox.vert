#version 420

layout (location = 0) in vec3 position;

uniform mat4 MVP;

out vec3 texCoord;

void main()
{
    gl_Position = (MVP * vec4(position, 1.0)).xyww;
    texCoord = normalize(position);
}