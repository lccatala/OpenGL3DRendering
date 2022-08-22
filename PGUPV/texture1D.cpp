
#include "texture1D.h"
#include "utils.h"
#include "log.h"
#include "image.h"

using PGUPV::Texture1D;
using PGUPV::Image;

Texture1D::Texture1D(GLenum texture_type, GLenum minfilter, GLenum magfilter,
	GLenum wrap_s)
	: Texture(texture_type, minfilter, magfilter, wrap_s), _width(0) {
	if (texture_type != GL_TEXTURE_1D)
		ERRT("Esta clase no soporta el tipo de textura pedida: " +
			PGUPV::hexString(texture_type));
	_name = "texture1d";
};


// Allocates memory for a texture with the given size and format
void Texture1D::allocate(uint width, GLint internalformat) {
	glBindTexture(_texture_type, _texId);
	setParams();
	if (internalformat == GL_RED || internalformat == GL_RG || internalformat == GL_RGB
		|| internalformat == GL_RGBA)
		glTexImage1D(_texture_type, 0, internalformat, width, 0, internalformat, GL_UNSIGNED_BYTE,
			NULL);
	else if (internalformat == GL_DEPTH_COMPONENT ||
		internalformat == GL_DEPTH_COMPONENT16 ||
		internalformat == GL_DEPTH_COMPONENT24 ||
		internalformat == GL_DEPTH_COMPONENT32)
		glTexImage1D(_texture_type, 0, internalformat, width, 0, GL_DEPTH_COMPONENT,
			GL_FLOAT, NULL);
	else if (internalformat == GL_DEPTH_STENCIL)
		glTexImage1D(_texture_type, 0, GL_DEPTH_STENCIL, width, 0,
			GL_DEPTH_STENCIL /* no debería importar */,
			GL_UNSIGNED_INT_24_8 /* no debería importar */, NULL);
	else
		glTexImage1D(_texture_type, 0, internalformat, width, 0,
			GL_RGBA /* no debería importar */,
			GL_BYTE /* no debería importar */, NULL);
	CHECK_GL2("Error reservando memoria para la textura");

	_width = width;
	_ready = true;
}

void Texture1D::setParams() {
	glTexParameteri(_texture_type, GL_TEXTURE_MAG_FILTER, _magfilter);
	glTexParameteri(_texture_type, GL_TEXTURE_MIN_FILTER, _minfilter);
	glTexParameteri(_texture_type, GL_TEXTURE_WRAP_S, _wrap_s);
}

void Texture1D::loadImageFromMemory(void *pixels, uint width,
	GLenum pixels_format, GLenum pixels_type,
	GLint internalformat) {
	_ready = false;
	/* Create and load textures to OpenGL */
	glBindTexture(_texture_type, this->_texId);
	setParams();

	glTexImage1D(_texture_type, 0, internalformat, width, 0, pixels_format,
		pixels_type, pixels);
	CHECK_GL();
	_width = width;

	_ready = true;
}

bool Texture1D::loadImage(const Image &image) {
	loadImageFromMemory(image.getPixels(), image.getWidth(), image.getGLFormatType(),
		image.getGLPixelBaseType(), image.getSuggestedGLInternalFormatType());
	return _ready;
}

bool Texture1D::loadImage(const std::string &filename) {
	PGUPV::Image image(filename);
	return loadImage(image);
}
