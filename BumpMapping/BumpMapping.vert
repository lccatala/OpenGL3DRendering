#version 420 core

$GLMatrices
$Lights

// Atributos del vértice (sistema de coordenadas del modelo)
in vec4 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;

out vec2 TexCoord;
out vec3 lightDirection, eyeDirection;

void main()
{
	TexCoord = vec2(texCoord);
	vec4 newPosition = modelviewMatrix * position;
	vec3 N = normalize(mat3(modelviewMatrix) * normal);
	vec3 T = normalize(mat3(modelviewMatrix) * vec3(1.0, 0.0, 0.0));
	vec3 B = cross(N, T);
	vec3 L = lights[0].positionEye.xyz - newPosition.xyz;
	vec3 V = -newPosition.xyz;

	lightDirection = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
	eyeDirection = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));

	gl_Position = projMatrix * newPosition;
}
