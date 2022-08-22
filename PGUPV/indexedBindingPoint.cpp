
#include <assert.h>

#include "bufferObject.h"
#include "indexedBindingPoint.h"
#include "log.h"

using PGUPV::IndexedBindingPoint;
using PGUPV::BufferObject;


IndexedBindingPoint
PGUPV::gl_transform_feedback_buffer(GL_TRANSFORM_FEEDBACK_BUFFER),
PGUPV::gl_uniform_buffer(GL_UNIFORM_BUFFER),
PGUPV::gl_atomic_counter_buffer(GL_ATOMIC_COUNTER_BUFFER),
PGUPV::gl_shader_storage_buffer(GL_SHADER_STORAGE_BUFFER);


std::shared_ptr<BufferObject> IndexedBindingPoint::bindBufferRange(std::shared_ptr<BufferObject> bo, GLuint index, GLintptr offset, GLsizeiptr size) {
  assert(bo);
  if (ulong(offset + size) > bo->getSize())
    ERRT("Intentando vincular una zona de memoria fuera del buffer");
  auto prev = getBound(index);

  glBindBufferRange(GL_bindingPoint, index, bo->getId(), offset, size);

  boundBOs[index] = bo;
  bound = bo;
  return prev;
}

std::shared_ptr<BufferObject> IndexedBindingPoint::bindBufferBase(std::shared_ptr<BufferObject> bo, GLuint index) {
  assert(bo);
  auto prev = getBound(index);
  glBindBufferBase(GL_bindingPoint, index, bo->getId());
  boundBOs[index] = bo;
  bound = bo;
  return prev;
}


std::shared_ptr<BufferObject> IndexedBindingPoint::getBound(GLuint idx) {
  auto p = boundBOs.find(idx);
  if (p != boundBOs.end()) {
    return p->second.lock();
  }
  return std::shared_ptr<BufferObject>();
}
