#version 420 core

// Este shader muestra el contenido del shadow map
// Un shadow map tiene valores entre 0 (en el plano de recorte frontal) y 1 (en
// el plano de recorte trasero). Para mostrar mejor los valores, se muestra  el
// rango entre 0 y 0.333 en una escala de rojos, los valores entre 0.333 y 0.666
// en una escala de verdes y el resto en una escala de azules.

in vec4 position;
in vec2 texCoord;

out vec2 texCoordFrag;

void main() {
  texCoordFrag = texCoord;
  gl_Position = position;
}
