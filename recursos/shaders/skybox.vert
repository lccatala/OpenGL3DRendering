#version 420

$GLMatrices

// Indica si debemos trabajar en un espacio levógiro
uniform bool levo = true;

in vec4 position;

out vec3 texCoord;

void main()
{
  texCoord = normalize(position.xyz);
  if (levo) texCoord *= vec3(1,1,-1);
  gl_Position =  modelviewprojMatrix * position;
}
