
#ifndef _TEXTURE2DARRAY_H
#define _TEXTURE2DARRAY_H 2014

#include <GL/glew.h>
#include "texture3DGeneric.h"

namespace PGUPV {

/* Esta clase representa un objeto textura. Permite cargar imágenes desde fichero
y establecer los parámetros para su uso.
*/
class Texture2DArray : public Texture3DGeneric {
public:
	// Constructor por defecto con unos parámetros iniciales (NO corresponden con los
	// valores por defecto de OpenGL
	Texture2DArray(GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
		GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT) :
		Texture3DGeneric(GL_TEXTURE_2D_ARRAY, minfilter, magfilter, wrap_s, wrap_t)
	{
		_name = "texture2darray";
	};
	virtual ~Texture2DArray() {}
};


};

#endif

