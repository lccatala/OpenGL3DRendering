#version 420 core

$Material
$GLMatrices

uniform vec4 lightpos; // lightpos (in eye space)

in vec3 Normal;
in vec4 vertexPos; // interpolated vertexPos (in eye space)

out vec4 final_color;

void main()
{
	vec4 color;
	vec4 amb;
	float intensity;
	vec3 lightDir;
	vec3 n;
	
	lightDir = normalize(vec3(lightpos-vertexPos));
	n = normalize(Normal);	
	intensity = max(dot(lightDir,n),0.0);
	
	color = diffuse;
	amb = ambient;

	final_color = (color * intensity) + amb;
}
