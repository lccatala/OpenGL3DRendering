#version 420

$Lights
$Material

uniform sampler2DShadow depthTexture;

in vec3 N, V;
in vec3 epos;
in vec4 spos;

out vec4 fragColor;

// Sólo se tiene en cuenta la primera fuente, y se considera que 
// es puntual.
vec4 iluminacion(vec3 pos, vec3 N, vec3 V, float f) {
  vec4 color = emissive + lights[0].ambient * ambient;
  // Vector iluminación (desde vértice a la fuente)
  vec3 L = normalize(vec3(lights[0].positionEye) - pos);
  // Multiplicador de la componente difusa
  float diffuseMult = max(dot(N,L), 0.0);
  float specularMult = 0.0;
  if (diffuseMult > 0.0) {
    // Multiplicador de la componente especular
    vec3 R = reflect(-L, N);
    specularMult = max(0.0, dot(R, V));
    specularMult = pow(specularMult, shininess);
  }
  color += f * (lights[0].diffuse * diffuse * diffuseMult +
         lights[0].specular * specular * specularMult);
  return color;
}

void main() {
  vec3 nN, nV;
  nN = normalize(N);
  nV = normalize(V);

  float f = textureProj(depthTexture, spos);
  fragColor = iluminacion(epos, nN, nV, f);
}
