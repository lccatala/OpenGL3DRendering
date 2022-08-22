#version 420

$GLMatrices

uniform mat4 shadowMatrix;

in vec4 position;
in vec3 normal;

out vec3 N, V;
out vec3 epos;
out vec4 spos;

void main() {
  // Normal en el espacio de la c�mara
  N = normalize(normalMatrix * normal);
  // V�rtice en el espacio de la c�mara
  epos = vec3(modelviewMatrix * position);
  // Posici�n del v�rtice en el espacio de clip escalado de la fuente
  spos = shadowMatrix * modelMatrix * position;
  // Vector vista (desde v�rtice a la c�mara)
  V = normalize(-epos);
  gl_Position = modelviewprojMatrix * position;
}
