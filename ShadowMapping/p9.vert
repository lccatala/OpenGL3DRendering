#version 420

$GLMatrices

uniform mat4 shadowMatrix;

in vec4 position;
in vec3 normal;

out vec3 N, V;
out vec3 epos;
out vec4 spos;

void main() {
  // Normal en el espacio de la cámara
  N = normalize(normalMatrix * normal);
  // Vértice en el espacio de la cámara
  epos = vec3(modelviewMatrix * position);
  // Posición del vértice en el espacio de clip escalado de la fuente
  spos = shadowMatrix * modelMatrix * position;
  // Vector vista (desde vértice a la cámara)
  V = normalize(-epos);
  gl_Position = modelviewprojMatrix * position;
}
