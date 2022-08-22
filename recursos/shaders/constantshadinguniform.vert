#version 420

$GLMatrices

in vec4 position;

void main()
{
  gl_Position = modelviewprojMatrix * position;
}
