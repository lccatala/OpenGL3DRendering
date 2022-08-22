#version 430 core

$GLMatrices

in vec4 position;
in vec3 normal;
in vec2 texcoords;

out vec2 TexCoords;
out vec3 ecPosition;
out vec3 ecNormal;

void main()
{
    TexCoords = texcoords;
    ecPosition = vec3(modelviewMatrix * position);
    ecNormal = normalMatrix * normal;   

    gl_Position =  modelviewprojMatrix * position;
}