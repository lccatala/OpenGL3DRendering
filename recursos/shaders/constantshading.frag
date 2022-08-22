#version 420

// Variable de entrada (el color interpolado de entre los vértices de la primitiva)
in vec4 fragcolor;
// Variable de salida: el color del fragmento
out vec4 final_color;

void main()
{
	// Simplemente copiamos la entrada en la salida
	final_color = fragcolor;
}
