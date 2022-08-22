
#ifndef _TEXTURERECTANGLE_H
#define _TEXTURERECTANGLE_H 2013


#include <GL/glew.h>
#include "texture2DGeneric.h"

namespace PGUPV {

/* Esta clase representa un objeto textura. Permite cargar imágenes desde fichero
y establecer los parámetros para su uso.
*/
class TextureRectangle : public Texture2DGeneric {
public:
	// Constructor por defecto con unos parámetros iniciales (NO corresponden con los
	// valores por defecto de OpenGL
	TextureRectangle(GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR):
		Texture2DGeneric(GL_TEXTURE_RECTANGLE, minfilter, magfilter, GL_REPEAT /* ignored */, GL_REPEAT /* ignored */) 
	{
		_name = "texturectangle";
	};
	virtual ~TextureRectangle() {};
};

};

#endif