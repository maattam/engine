//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Renders depth to texture.
//

#version 420

uniform sampler2D gMaskSampler;

in vec2 texCoord0;

void main()
{
    if(texture(gMaskSampler, texCoord0).r <= 0.2)
        discard;
}