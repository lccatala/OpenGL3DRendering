#version 420

$GLMatrices
$Lights
$Material

in vec4 position;
in vec3 normal;

out vec3 fragN;
out vec3 fragPosition;

void main() {
  gl_Position = modelviewprojMatrix * position;
  fragPosition = (modelviewMatrix * position).xyz;
  fragN = normalize(normalMatrix * normal);
}
