#pragma once

#include "texture2D.h"


/**
\class TextureGenerator
Clase de ayuda que genera texturas procedurales.
*/

namespace PGUPV
{
	class TextureGenerator {
	public:
		/**
		Devuelve una textura de tablero de ajedrez
		\param color1 color de la casilla de la esquina inferior izquierda
		\param color2 color de la siguiente casilla
		\param size Tama�o en p�xeles del lado de la textura (ser� cuadrada)
		\param numsquares N�mero de casillas en cada dimensi�n

		*/
		static Texture2D *makeChecker(const glm::vec4 &color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			const glm::vec4 &color2 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), unsigned int size = 256, 
			unsigned int numsquares = 8);

	};
}