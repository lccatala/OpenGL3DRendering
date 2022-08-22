#version 420 core

uniform sampler2D texUnit;
uniform vec3 colorModifier;

in vec2 texCoordFrag;
out vec4 fragColor;

void main() { 
  vec4 oc = texture(texUnit, texCoordFrag); 
  float lum = 0.2;
  float m = floor(mod(gl_FragCoord.y, 4));
  if (floor(mod(gl_FragCoord.y, 4)) == 0.0)
  {
	lum = 1.0;
  }
  else if (floor(mod(gl_FragCoord.y + 1, 4)) == 0.0)
  {
	lum = 0.2;
  }
  else if (floor(mod(gl_FragCoord.y + 2, 4)) == 0.0)
  {
	lum = 0.2;
  }
  else
  {
	lum = 0.8;
  }

  float greyValue = (0.3 * oc.x + 0.59 * oc.y + 0.11 * oc.z) * lum;
  fragColor = vec4(greyValue * colorModifier.r, greyValue * colorModifier.g, greyValue * colorModifier.b, 1.0);
}
