#ifndef _INDEXED_BINDING_POINT_H
#define _INDEXED_BINDING_POINT_H 2014

#include <map>
#include "bindingPoint.h"

#include "log.h"

namespace PGUPV {
  class IndexedBindingPoint : public BindingPoint {
  public:
    explicit IndexedBindingPoint(GLenum GL_BP) : BindingPoint(GL_BP) {};
    /** Vincula el B.O. al índice del punto de vinculación indicado. Internamente
    usa la función glBindBufferBase
    \param bo: buffer object a vincular
    \param index: índice del punto de vinculación
    \returns un puntero compartido (que puede estar vacío) al B.O. que estaba
    previamente vinculado a este punto
    */
    std::shared_ptr<BufferObject> bindBufferBase(std::shared_ptr<BufferObject> bo,
      GLuint index);
    /** Vincula una porción del B.O. al índice del punto de vinculación indicado.
    Internamente usa la función
    glBindBufferRange
    \param bo: buffer object a vincular
    \param index: índice del punto de vinculación
    \param offset: posición del primer byte a vincular dentro del B.O.
    \param size: tamaño de la región a vincular
    \returns un puntero compartido (que puede estar vacío) al B.O. que estaba
    previamente vinculado a este punto
    */
    std::shared_ptr<BufferObject>
      bindBufferRange(std::shared_ptr<BufferObject> bo, GLuint index,
      GLintptr offset, GLsizeiptr size);

    /**
    Devuelve una referencia al BufferObject vinculado en el índice indicado. Puede ser
    que no haya B.O. vinculados y el puntero esté vacío.
    \param idx el índice a consultar
    \return el buffer object, o un puntero vacío
    */
    std::shared_ptr<BufferObject> getBound(GLuint idx);
  private:
    std::map<GLuint, std::weak_ptr<BufferObject>> boundBOs;

  };

  extern IndexedBindingPoint
    gl_transform_feedback_buffer,
    gl_uniform_buffer,
    gl_atomic_counter_buffer, // GL 4.2
    gl_shader_storage_buffer; // GL 4.3
};

#endif
