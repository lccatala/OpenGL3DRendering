#version 420 core

$GLMatrices

in vec3 position;
in vec2 texCoord;

out vec2 texCoordFrag;

void main()
{
	texCoordFrag = texCoord;
	gl_Position = modelviewprojMatrix *  vec4(position, 1.0) ;
}