#version 330 core

in vec3 texCoord0;

out vec4 fragColor;

uniform samplerCube gCubemapTexture;

void main()
{
    fragColor = texture(gCubemapTexture, texCoord0);
}