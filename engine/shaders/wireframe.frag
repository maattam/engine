//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Wireframe debugging shader.
//

#version 420

uniform sampler2D gDiffuseSampler;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;

in vec2 texCoord0;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture2D(gDiffuseSampler, texCoord0) * vec4(diffuseColor, 1.0) + vec4(ambientColor, 1.0);
}