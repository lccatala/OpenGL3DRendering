#version 420 core

uniform sampler2D texUnit;

// Usar una escala de grises para mostrar el contenido de la textura,
// o usar una escala de colores
uniform bool grayScale = true;

in vec2 texCoordFrag;
out vec4 fragColor;

// Recibe un número entre 0 y 1/3, y devuelve otro entre 0.5 y 1
float ss(float f) { return f * 0.5 / 0.3; }

void main() {
  float z = texture(texUnit, texCoordFrag).x;
  if (grayScale)
    fragColor = vec4(z, z, z, 1.0);
  else {
    if (z == 0.0)
      fragColor = vec4(0, 0, 0, 1);
    else if (z == 1.0)
      fragColor = vec4(1, 1, 1, 1);
    else if (z <= 0.333)
      fragColor = vec4(ss(z), 0, 0, 1);
    else if (z <= 0.666)
      fragColor = vec4(0, ss(z - 1.0 / 3), 0, 1);
    else
      fragColor = vec4(0, 0, ss(z - 2.0 / 3), 1.0);
  }
}
