//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Forward stage fragment shader
//

#version 420

layout(location = 0) out vec4 fragColor;

in vec2 texCoord0;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    float alpha;
    
    sampler2D diffuseSampler;
    sampler2D maskSampler;
};

uniform Material material;
uniform sampler2DMS depth;

void main()
{
    // Check if this part of the fragment is opaque according to mask
    if(texture(material.maskSampler, texCoord0).r <= 0.2)
        discard;

    ivec2 st = ivec2(gl_FragCoord.xy);

    // Discard occluded fragments
    if(gl_FragCoord.z > texelFetch(depth, st, 0).x)
        discard;

    fragColor.rgb = texture(material.diffuseSampler, texCoord0).rgb * material.diffuse
                        + material.ambient;
    fragColor.a = material.alpha;
}