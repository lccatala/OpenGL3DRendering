#version 420 core

$GLMatrices

in vec4 position;
in vec3 normal;

out vec4 vertexPos;
out vec3 Normal;

void main()
{
	Normal = normalize(normalMatrix * normal);
	vertexPos = modelviewMatrix * position;
	gl_Position = projMatrix * vertexPos;
	
}