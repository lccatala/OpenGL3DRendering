
#include "bufferObject.h"
#include "bindingPoint.h"

#include "log.h"

using PGUPV::BufferObject;

BufferObject::BufferObject(size_t size, GLenum usage)
	: size(size), usage(usage) {
	glGenBuffers(1, &id);
	if (id == 0)
		ERRT("No se ha podido crear el buffer. Inicializa OpenGL antes");
}

BufferObject::~BufferObject() { 
  glDeleteBuffers(1, &id); 
 
  INFO(getName() + " destruído");
}

std::shared_ptr<BufferObject> BufferObject::build(size_t size, GLenum usage) {
	std::shared_ptr<BufferObject> bo =
		std::shared_ptr<BufferObject>(new BufferObject(size, usage));
	allocate(bo);
	return bo;
}

std::shared_ptr<BufferObject> BufferObject::buildImmutable(ulong size, GLenum usage) {
	std::shared_ptr<BufferObject> bo =
		std::shared_ptr<BufferObject>(new BufferObject(size, usage));
	allocateImmutable(bo);
	return bo;
}

void BufferObject::allocate(std::shared_ptr<BufferObject> bo) {
	assert(bo);
	std::shared_ptr<BufferObject> prev = PGUPV::gl_copy_write_buffer.bind(bo);
	glBufferData(GL_COPY_WRITE_BUFFER, bo->size, NULL, bo->usage);
	PGUPV::gl_copy_write_buffer.bind(prev);
  INFO(bo->getName() + " ha reservado " + std::to_string(bo->size) + " bytes");
}

void BufferObject::allocateImmutable(std::shared_ptr<BufferObject> bo) {
	assert(bo);
	std::shared_ptr<BufferObject> prev = PGUPV::gl_copy_write_buffer.bind(bo);
	glBufferStorage(GL_COPY_WRITE_BUFFER, bo->size, NULL, bo->usage);
	PGUPV::gl_copy_write_buffer.bind(prev);
	INFO(bo->getName() + " (Inmutable) ha reservado " + std::to_string(bo->size) + " bytes");
}

void BufferObject::setGlDebugLabel(const std::string &label) {
  debugLabel = label;
  if (glObjectLabel) {
    glObjectLabel(GL_BUFFER, id, -1, debugLabel.c_str());
  }
}


std::string BufferObject::getName() {
  std::string msg("Buffer object " + std::to_string(id));
  if (!debugLabel.empty()) {
    msg += " (" + debugLabel + ")";
  }
  return msg;
}
