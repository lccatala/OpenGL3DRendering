#version 420 core

$GLMatrices

in vec4 position;
in vec2 texCoord;

out vec2 texCoordFrag;

void main() {
  texCoordFrag = texCoord;
  gl_Position = modelviewprojMatrix * position;
}
