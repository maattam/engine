#version 330 core

in vec3 texCoord0;

out vec4 fragColor;

uniform samplerCube gCubemapTexture;

void main()
{
    // Simulate HDR Skybox.. todo proper implementation using masks?
    fragColor = 3.0 * texture(gCubemapTexture, texCoord0);
}