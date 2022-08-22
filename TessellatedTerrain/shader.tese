#version 420 core

layout (quads, equal_spacing, ccw) in;

$GLMatrices

uniform	sampler2D texUnitHeightMap;

in TESC_OUT {
	vec2 textureCoord;
} from_tesc[];

out TESE_OUT {
	vec2 textureCoord;
};
void main() {
	vec2 t1 = mix(from_tesc[0].textureCoord, from_tesc[1].textureCoord, gl_TessCoord.x);
	vec2 t2 = mix(from_tesc[3].textureCoord, from_tesc[2].textureCoord, gl_TessCoord.x);

	textureCoord = mix(t1, t2, gl_TessCoord.y);

	// Calculamos la posición del vértice teselado como una interpolación bilineal
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);

	vec4 texColor = texture2D(texUnitHeightMap, textureCoord);
	vec4 pos = mix(p1, p2, gl_TessCoord.y);
	float heightFactor = 2.0;
	float height = texColor.r * heightFactor;
	pos.y = height;

	gl_Position = modelviewprojMatrix * pos;
}
