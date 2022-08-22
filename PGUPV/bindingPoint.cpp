#include "bindingPoint.h"
#include "utils.h"
#include "bufferObject.h"
#include "log.h"

using PGUPV::BindingPoint;
using PGUPV::BufferObject;

BindingPoint PGUPV::gl_array_buffer(GL_ARRAY_BUFFER),
PGUPV::gl_copy_read_buffer(GL_COPY_READ_BUFFER),
PGUPV::gl_copy_write_buffer(GL_COPY_WRITE_BUFFER),
PGUPV::gl_element_array_buffer(GL_ELEMENT_ARRAY_BUFFER),
PGUPV::gl_pixel_pack_buffer(GL_PIXEL_PACK_BUFFER),
PGUPV::gl_pixel_unpack_buffer(GL_PIXEL_UNPACK_BUFFER),
PGUPV::gl_query_buffer(GL_QUERY_BUFFER), 
PGUPV::gl_texture_buffer(GL_TEXTURE_BUFFER),
PGUPV::gl_draw_indirect_buffer(GL_DRAW_INDIRECT_BUFFER), // GL 4.0
PGUPV::gl_dispatch_indirect_buffer(GL_DISPATCH_INDIRECT_BUFFER); // GL 4.3

std::shared_ptr<BufferObject> BindingPoint::bind(std::shared_ptr<BufferObject> bo) {
	std::shared_ptr<BufferObject> prev = bound.lock();
	glBindBuffer(GL_bindingPoint, bo ? bo->getId() : 0);
	bound = bo;
	return prev;
}

std::shared_ptr<BufferObject> BindingPoint::unbind() {
	return bind(std::shared_ptr<BufferObject>());
}

void BindingPoint::read(void *dst) {
	read(dst, 0, 0);
}

void BindingPoint::read(void *dst, size_t size, uint offset) {
	if (auto bo = bound.lock()) {
		if (offset + size > bo->getSize()) 
			ERRT("Intentando leer más allá del fin del buffer");
		if (size == 0) {
			size = bo->getSize() - offset;
		}
		glGetBufferSubData(GL_bindingPoint, offset, size, dst);
	} else
		ERRT("No hay un buffer vinculado desde donde leer");
}

void BindingPoint::write(const void *src) {
	if (auto bo = bound.lock()) {
		glBufferSubData(this->GL_bindingPoint, 0, bo->getSize(), src);
	} else 
		ERRT("No se puede escribir sin un buffer vinculado");
	CHECK_GL2("Error al escribir en el Buffer object");
}

void BindingPoint::write(const void *src, ulong size, ulong offset) {
	std::shared_ptr<BufferObject> curr;
	if (!(curr = bound.lock())) 
		ERRT("No se puede escribir sin un buffer vinculado");

	if (offset + size > curr->getSize()) 
		ERRT("Estas intentando escribir fuera de la memoria reservada del Buffer Object!");

	glBufferSubData(GL_bindingPoint, offset, size, src);
	CHECK_GL2("Error al escribir en el Buffer object");
}

void BindingPoint::copy(GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
	std::shared_ptr<BufferObject> readFrom, writeTo;
	if (!(readFrom = gl_copy_read_buffer.bound.lock())) 
		ERRT("No hay un buffer vinculado al punto de lectura de la copia");
	if (!(writeTo = gl_copy_write_buffer.bound.lock()))
		ERRT("No hay un buffer vinculado al punto de escritura de la copia");
	if (ulong(readOffset + size) > readFrom->getSize())
		ERRT("Estas desbordando el buffer de lectura");
	if (ulong(writeOffset + size) > writeTo->getSize())
		ERRT("Estas desbordando el buffer de escritura");

	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
	CHECK_GL();
}

void *BindingPoint::map(GLenum usage) {
	std::shared_ptr<BufferObject> curr;
	if (!(curr = bound.lock()))
		ERRT("No se puede mapear por que no hay un buffer object vinculado");
	
	return glMapBuffer(GL_bindingPoint, usage);
}


void *BindingPoint::map(ulong offset, ulong size, GLbitfield access) {
	std::shared_ptr<BufferObject> curr;
	if (!(curr = bound.lock()))
		ERRT("No se puede mapear por que no hay un buffer object vinculado");

	return glMapBufferRange(GL_bindingPoint, offset, size, access);
}

void BindingPoint::unmap() {
	std::shared_ptr<BufferObject> curr;
	if (!(curr = bound.lock()))
		ERRT("No se puede desmapear por que no hay un buffer object vinculado");

	glUnmapBuffer(GL_bindingPoint);
}


void BindingPoint::clear(GLenum internalformat, GLintptr offset, GLsizeiptr size,
	GLenum format, GLenum type, const void *data) {
	std::shared_ptr<BufferObject> curr;

	if (!(curr = bound.lock()))
		ERRT("No se puede escribir sin un buffer vinculado");

	if (ulong(offset + size) > curr->getSize())
		ERRT("Estas intentando escribir fuera de la memoria reservada del Buffer Object!");

	glClearBufferSubData(GL_bindingPoint, internalformat, offset, size, format, type, data);
}
