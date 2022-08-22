#version 420

// Todos los fragmentos de la primitiva tendrán el color definido por esta variable:
uniform vec4 primitive_color;

// Color final
out vec4 final_color;

void main()
{
  final_color = primitive_color;
}
