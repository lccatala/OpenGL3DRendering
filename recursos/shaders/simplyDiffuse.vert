#version 420

$GLMatrices

in vec3 position;
in vec3 normal;


// Posición de la fuente en el espacio de la cámara
uniform vec3 lightpos;
// Color difuso del objeto
uniform vec4 diffuseColor;

out vec4 fragColor;

void main() {
	// Normal en el espacio de la cámara
	vec3 eN = normalize(normalMatrix *normal);
	// Vértice en el espacio de la cámara
	vec4 pos4 = vec4(position, 1.0);
	vec3 eposition=vec3(modelviewMatrix * pos4);
	// Cálculo de la iluminación
	fragColor = max(0.0, dot(eN, normalize(lightpos-eposition))) * diffuseColor;
	gl_Position = modelviewprojMatrix * pos4;
}
