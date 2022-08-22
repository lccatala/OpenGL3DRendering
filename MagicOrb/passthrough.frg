#version 420 core

uniform sampler2D texUnit;

in vec2 texCoordFrag;
out vec4 fragColor;

void main() { 
  fragColor = texture(texUnit, texCoordFrag); 
}
