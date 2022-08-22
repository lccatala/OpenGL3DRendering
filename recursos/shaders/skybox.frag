#version 420

in vec3 texCoord;
out vec4 finalColor;
uniform samplerCube cubeMap;

void main() {
  // El color final es el color del texel apuntado por tc
  finalColor = texture(cubeMap, texCoord);	
}
