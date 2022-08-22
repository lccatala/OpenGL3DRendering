
#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H 2011

#include "texture2DGeneric.h"

namespace PGUPV {

  /* Esta clase representa un objeto textura. Permite cargar imágenes desde fichero
  y establecer los parámetros para su uso.
  */
  class Texture2D : public Texture2DGeneric {
  public:
    // Constructor por defecto con unos parámetros iniciales (NO corresponden con los
    // valores por defecto de OpenGL
    Texture2D(GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
      GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT)
      : Texture2DGeneric(GL_TEXTURE_2D, minfilter, magfilter, wrap_s, wrap_t) {};
    virtual ~Texture2D() {}
  };

};

#endif
