#version 420 core

// Rectangle Texture que contiene:
// el depth buffer, en el canal rojo
// el stencil buffer, en el RGB
uniform sampler2DRect stencilOrDepthUnit;
// showStencil == 1 -> mostrar stencil, showStencil!=1 -> mostrar depth
uniform int showStencil;

out vec4 fragColor;

void main()
{
	if (showStencil != 1) {
		float depth = texture(stencilOrDepthUnit, gl_FragCoord.xy).x;
		fragColor = vec4(vec3(depth), 1.0);
	} else {
		fragColor = texture(stencilOrDepthUnit, gl_FragCoord.xy);
	}
}
