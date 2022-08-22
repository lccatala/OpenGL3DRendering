#version 430 core

$GLMatrices
$PBRLights
$PBRMaterial

out vec4 FragColor;

in vec2 TexCoords;
in vec3 ecPosition;
in vec3 ecNormal;


void main()
{		
    FragColor = texture(baseColorMap, TexCoords);

}
