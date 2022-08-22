
#ifndef PGUPV_bufferTexture_h
#define PGUPV_bufferTexture_h 2013

#include <memory>
#include "bufferObject.h"
#include "bindableTexture.h"

namespace PGUPV {

/**
\class BufferTexture
Esta clase da acceso a un buffer object mediante las funciones de una textura.
*/

class BufferTexture : public BindableTexture {
public:
  /**
  Crea un nuevo BufferTexture
  \param internalFormat formato interno de la textura (p.e., GL_RGB8,
  GL_RGBA32f, etc)
  \param size tamaño del buffer object en bytes
  \param data puntero a los datos a copiar en el buffer
  \param usage uso que se le dará al buffer (por defecto, GL_STATIC_DRAW)
  */
  BufferTexture(GLenum internalFormat, size_t size, void *data,
                GLenum usage = GL_STATIC_DRAW);

  /**
  Crea un nuevo BufferTexture a partir de un Buffer Object existente.
  \param internalFormat formato interno de la textura (p.e., GL_RGB8,
  GL_RGBA32f, etc)
  \param bo buffer object a usar por este BufferTexture
  */
  BufferTexture(GLenum internalFormat, std::shared_ptr<BufferObject> bo);
  /**
	No se puede borrar una textura de este tipo con glClearTexImage
  */
  virtual void clear(int level, GLenum format, GLenum type, const void *data);
private:
  std::shared_ptr<BufferObject> bo;
  GLenum _internalFormat;
#ifdef _DEBUG
  static const GLenum acceptedFormats[];
#endif
};
};

#endif
