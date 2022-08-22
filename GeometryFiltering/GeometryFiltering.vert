#version 420

$GLMatrices

in vec3 position;
in vec3 normal;


// Posici�n de la fuente en el espacio de la c�mara
uniform vec3 lightpos;
// Color difuso del objeto
uniform vec4 diffuseColor;

out vec4 color;

void main() {
	// Normal en el espacio de la c�mara
	vec3 eN = normalize(normalMatrix *normal);
	// V�rtice en el espacio de la c�mara
	vec4 pos4 = vec4(position, 1.0);
	vec3 eposition=vec3(modelviewMatrix * pos4);
	// C�lculo de la iluminaci�n
	color = max(0.0, dot(eN, normalize(lightpos-eposition))) * diffuseColor;
	gl_Position = modelviewMatrix * pos4;
}
