#version 420 core

$Lights

layout (binding=$TEXDIFF) uniform sampler2D colores;
layout (binding=$TEXNORM) uniform sampler2D normales;
layout (binding=$TEXSPEC) uniform sampler2D brillos;
layout (binding=$TEXHEIGHT) uniform sampler2D alturas;

uniform bool useParallax;
uniform float scale, bias;

in vec2 TexCoord;
in vec3 lightDirection, eyeDirection;
out vec4 fragColor;


void main()
{
	vec2 T = TexCoord;
	vec3 V = normalize(eyeDirection);
	vec3 L = normalize(lightDirection);

	if (useParallax)
	{
		T = T + texture(alturas, T).r * scale - bias * V.xy;
	}

	vec3 N = normalize(texture(normales, T).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec3 R = reflect(-L, N);

	vec3 diffuseAlbedo = texture(colores, T).rgb;
	vec3 diffuse = max(dot(N, L), 0.0) * diffuseAlbedo;

	vec3 specularAlbedo = texture(brillos, T).rgb;
	vec3 specular = max(pow(dot(R, V), 2.0), 0.0) * specularAlbedo;
	fragColor = vec4(diffuse + specular, 1.0);
}
