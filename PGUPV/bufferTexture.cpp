#include <glm/glm.hpp>

#include "bufferTexture.h"
#include "bindingPoint.h"
#include "utils.h"
#include "log.h"

using PGUPV::BufferTexture;
using PGUPV::BufferObject;

#ifdef _DEBUG
static const GLenum acceptedFormats[] = {
	GL_R8, GL_R16, GL_R16F, GL_R32F, GL_R8I, GL_R16I,
	GL_R32I, GL_R8UI, GL_R16UI, GL_R32UI, GL_RG8, GL_RG16,
	GL_RG16F, GL_RG32F, GL_RG8I, GL_RG16I, GL_RG32I, GL_RG8UI,
	GL_RG16UI, GL_RG32UI, GL_RGBA8, GL_RGBA16, GL_RGBA16F, GL_RGBA32F,
	GL_RGBA8I, GL_RGBA16I, GL_RGBA32I, GL_RGBA8UI, GL_RGBA16UI, GL_RGBA32UI };

static void checkFormatAccepted(GLenum format) {
	// Comprobar que el formato interno es correcto (tiene tamaño)
	int i, sizeAcceptedFormats = sizeof(acceptedFormats) / sizeof(acceptedFormats[0]);
	for (i = 0; i < sizeAcceptedFormats; i++) {
		if (format == acceptedFormats[i])
			break;
	}
	if (i == sizeAcceptedFormats) {
		ERRT("Formato interno no aceptado. Debe especificar el tipo y tamaño de "
			"los componentes");
	}
}
#endif

BufferTexture::BufferTexture(GLenum internalFormat, size_t size, void *data,
	GLenum usage)
	: BindableTexture(GL_TEXTURE_BUFFER), bo(BufferObject::build(size, usage)),
	_internalFormat(internalFormat) {
#ifdef _DEBUG
	checkFormatAccepted(internalFormat);
#endif
	PGUPV::gl_texture_buffer.bind(bo);
	PGUPV::gl_texture_buffer.write(data);
	glBindTexture(GL_TEXTURE_BUFFER, getId());
	glTexBuffer(GL_TEXTURE_BUFFER, _internalFormat, bo->getId());
	_ready = true;
}

BufferTexture::BufferTexture(GLenum internalFormat,
	std::shared_ptr<BufferObject> bo)
	: BindableTexture(GL_TEXTURE_BUFFER), bo(bo),
	_internalFormat(internalFormat) {
#ifdef _DEBUG
	checkFormatAccepted(internalFormat);
#endif
	PGUPV::gl_texture_buffer.bind(bo);
	glBindTexture(GL_TEXTURE_BUFFER, getId());
	glTexBuffer(GL_TEXTURE_BUFFER, _internalFormat, bo->getId());
	_ready = true;
}

void BufferTexture::clear(int /*level*/, GLenum /*format*/, GLenum /*type*/, const void * /*data*/) {
	ERRT("No se puede borrar una textura de tipo Buffer Texture");
}
