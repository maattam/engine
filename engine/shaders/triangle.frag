#version 330 core

uniform sampler2D	texture;
uniform vec3		LightColor;
uniform float		LightPower;
uniform vec3		LightPosition_worldspace;
uniform vec3		MaterialAmbientColor;
uniform vec3		MaterialSpecularColor;
uniform vec3		DiffuseColor;

in vec2 texcoord;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

layout(location = 0) out vec3 color;

void main() {
	vec3 MaterialDiffuseColor = texture2D(texture, texcoord).rgb * DiffuseColor;

	// Distance to the light
	float distance = length(LightPosition_worldspace - Position_worldspace);
	float dist2 = 1 / (distance * distance);

	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);
	vec3 e = normalize(EyeDirection_cameraspace);

	// Direction in which the triangle reflects the light
	vec3 r = reflect(-l, n);

	float cosAlpha = clamp(dot(e, r), 0, 1);
	float cosTheta = clamp(dot(n, l), 0, 1);

	color = MaterialAmbientColor +
			(MaterialDiffuseColor * cosTheta + MaterialSpecularColor * pow(cosAlpha, 5))
			* LightColor * LightPower * dist2;
}