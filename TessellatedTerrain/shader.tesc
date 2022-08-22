#version 420 core

layout (vertices = 4) out;

in VS_OUT {
	vec2 textureCoord;
} from_vs[];

out TESC_OUT {
	vec2 textureCoord;
} to_tese[];


$GLMatrices

void main() {
	// Propagamos la posici�n y la coordenada de textura al shader de evaluaci�n
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	to_tese[gl_InvocationID].textureCoord = from_vs[gl_InvocationID].textureCoord;

	vec3 eposition = vec3(modelviewMatrix * gl_in[gl_InvocationID].gl_Position);
	float d = length(eposition);
	float factor = 64.0;
	float level = 1.0 + factor / d;

	/*
		Calcula aqu� los niveles de teselaci�n
	*/
	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = 1.0 + factor / (length(vec3(modelviewMatrix * gl_in[0].gl_Position)) + length(vec3(modelviewMatrix * gl_in[3].gl_Position))) / 2;
		gl_TessLevelOuter[1] = 1.0 + factor / (length(vec3(modelviewMatrix * gl_in[1].gl_Position)) + length(vec3(modelviewMatrix * gl_in[0].gl_Position))) / 2;;
		gl_TessLevelOuter[2] = 1.0 + factor / (length(vec3(modelviewMatrix * gl_in[2].gl_Position)) + length(vec3(modelviewMatrix * gl_in[1].gl_Position))) / 2;;
		gl_TessLevelOuter[3] = 1.0 + factor / (length(vec3(modelviewMatrix * gl_in[3].gl_Position)) + length(vec3(modelviewMatrix * gl_in[2].gl_Position))) / 2;;

		gl_TessLevelInner[0] = level;
		gl_TessLevelInner[1] = level;
	}
}
