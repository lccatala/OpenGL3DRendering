#version 420 core

uniform sampler2D texUnit0, texUnit1;
uniform vec3 filterColor;

in vec2 texCoordFrag;
out vec4 fragColor;

void main() { 
  fragColor = texture(texUnit1, texCoordFrag); 
  if (fragColor.g > filterColor.g) {
	fragColor.b = fragColor.g;
	fragColor.g = filterColor.b;
	return;
  }
  else if (fragColor.r < filterColor.r) {
	discard;
  }
}
