#version 420 core

// Para Texture 2d normales...
uniform sampler2D textureUnit;
// Para Rectangle Texture
//uniform sampler2DRect Texture;

in vec2 texCoordFrag;
out vec4 fragColor;
uniform vec4 foregroundColor;
uniform vec4 backgroundColor;

// 
// ????X -> 1: dibujar fondo sólido
// ???X? -> 1: modo de dibujo de texto (la textura contiene un texto)
uniform int drawingMode;


bool solidBackgroundMode() {
	return (drawingMode & 1) != 0;
}

bool textMode() {
	return (drawingMode & 2) != 0;
}

void main()
{
	if (textMode()) {
		vec4 color = texture(textureUnit, texCoordFrag);
		if (color.r == 0.0) {
			if (solidBackgroundMode()) 
				fragColor = backgroundColor;
			else 
				discard;
		} else 
			fragColor = foregroundColor;
	} else {
		fragColor = foregroundColor;
	}
}
