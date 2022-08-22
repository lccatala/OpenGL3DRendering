
#ifndef _BUFFER_OBJECT_H
#define _BUFFER_OBJECT_H

/** \file bufferObject.h

\author Paco Abad

*/

#include <memory>
#include <GL/glew.h>

#include "common.h"

namespace PGUPV {

  /**
  \class BufferObject

  Clase que representa un buffer object en una implementación OpenGL.

  */
  class BufferObject {
  public:
    /**
    Factoría de BufferObjects. Necesita el tamaño en bytes del B.O.
    Se le puede pasar el tipo de uso que se le va a dar al buffer (por defecto
    será GL_DYNAMIC_DRAW).
    \param size Tamaño del buffer a reservar (en bytes)
    \param usage El uso que se le va a dar al buffer (GL_STATIC_DRAW para información estática que se
    usará para dibujar, GL_DYNAMIC_DRAW si dicha información cambiará frecuentemente, etc).
    */
    static std::shared_ptr<BufferObject> build(size_t size, GLenum usage = GL_DYNAMIC_DRAW);

    /**
    Factoría de BufferObjects. Necesita el tamaño en bytes del B.O.
    Se le puede pasar el tipo de uso que se le va a dar al buffer (por defecto
    será GL_DYNAMIC_DRAW). Construye un B.O. inmutable (reservado con glBufferStorage)
    \param size Tamaño del buffer a reservar (en bytes)
    \param usage El uso que se le va a dar al buffer (http://docs.gl/gl4/glBufferStorage).
    \warning Necesita OpenGL 4.4
    */
    static std::shared_ptr<BufferObject> buildImmutable(ulong size, GLenum usage = GL_DYNAMIC_STORAGE_BIT);
    /** Devuelve el identificador del B.O. */
    GLuint getId() const  { return this->id; };
    /** Devuelve el tamaño que se pidió al crear el buffer object */
    size_t getSize() const { return this->size; };
    /**
    Establece una etiqueta que se mostrará en los mensajes de depuración de GL (GL 4.3)
    \param label Etiqueta a mostrar para este buffer
    */
    void setGlDebugLabel(const std::string &label);
    /**
    \return La etiqueta que muestra OpenGL en los mensajes de depuración de este objeto
    */

    std::string getGlDebugLabel() const { return debugLabel; };

    virtual ~BufferObject();
  protected:
    /** Constructor.
     */
    BufferObject(size_t size, GLenum usage);
    static void allocate(std::shared_ptr<BufferObject> bo);
    static void allocateImmutable(std::shared_ptr<BufferObject> bo);
    // No se permite copiar el objeto
    BufferObject(const BufferObject &other) = delete;
    BufferObject& operator=(BufferObject other) = delete;

    std::string getName();
    GLuint id;
    size_t size;
    GLenum usage;
    std::string debugLabel;
  };

};

#endif
