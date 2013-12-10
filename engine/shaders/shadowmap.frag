#version 330 core

uniform sampler2D gMaskSampler;

in vec2 texCoord0;

void main()
{
    if(texture2D(gMaskSampler, texCoord0).r <= 0.2)
        discard;
}