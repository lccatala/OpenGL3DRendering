// Shader de vértice para generar el shadow map
#version 420 core

$GLMatrices

in vec4 position;

void main(void) { 
  gl_Position = modelviewprojMatrix * position;
}
