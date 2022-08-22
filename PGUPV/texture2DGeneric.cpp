
#include "app.h"
#include "texture2DGeneric.h"
#include "utils.h"
#include "log.h"
#include "image.h"

using PGUPV::Texture2DGeneric;
using PGUPV::Image;

Texture2DGeneric::Texture2DGeneric(GLenum texture_type, GLenum minfilter,
	GLenum magfilter, GLenum wrap_s,
	GLenum wrap_t)
	: Texture(texture_type, minfilter, magfilter, wrap_s, wrap_t), _width(0),
	_height(0) {
	if (texture_type != GL_TEXTURE_2D && texture_type != GL_PROXY_TEXTURE_2D &&
		texture_type != GL_TEXTURE_1D_ARRAY &&
		texture_type != GL_PROXY_TEXTURE_1D_ARRAY &&
		texture_type != GL_TEXTURE_RECTANGLE &&
		texture_type != GL_PROXY_TEXTURE_RECTANGLE &&
		texture_type != GL_TEXTURE_CUBE_MAP_POSITIVE_X &&
		texture_type != GL_TEXTURE_CUBE_MAP_NEGATIVE_X &&
		texture_type != GL_TEXTURE_CUBE_MAP_POSITIVE_Y &&
		texture_type != GL_TEXTURE_CUBE_MAP_NEGATIVE_Y &&
		texture_type != GL_TEXTURE_CUBE_MAP_POSITIVE_Z &&
		texture_type != GL_TEXTURE_CUBE_MAP_NEGATIVE_Z &&
		texture_type != GL_PROXY_TEXTURE_CUBE_MAP)
		ERRT("Esta clase no soporta el tipo de textura pedida: " +
			PGUPV::hexString(texture_type));
};

// Allocates memory for a texture with the given size and format
void Texture2DGeneric::allocate(uint width, uint height, GLint internalformat) {
	glBindTexture(_texture_type, _texId);
	setParams();
	if (internalformat == GL_RED || internalformat == GL_RG || internalformat == GL_RGB
		|| internalformat == GL_RGBA)
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0, internalformat,
			GL_UNSIGNED_BYTE, nullptr);
	else if (internalformat == GL_DEPTH_COMPONENT ||
		internalformat == GL_DEPTH_COMPONENT16 ||
		internalformat == GL_DEPTH_COMPONENT24 ||
		internalformat == GL_DEPTH_COMPONENT32)
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	else if (internalformat == GL_DEPTH_STENCIL)
		glTexImage2D(_texture_type, 0, GL_DEPTH_STENCIL, width, height, 0,
			GL_DEPTH_STENCIL /* no debería importar */,
			GL_UNSIGNED_INT_24_8 /* no debería importar */, nullptr);
	else if (internalformat == GL_R32UI || internalformat == GL_R32I)
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
			GL_RED_INTEGER, internalformat == GL_R32UI ? GL_UNSIGNED_INT : GL_INT, nullptr);
	else if (internalformat == GL_R16UI || internalformat == GL_R16I)
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
			GL_RED_INTEGER, internalformat == GL_R16UI ? GL_UNSIGNED_SHORT : GL_SHORT, nullptr);
	else if (internalformat == GL_R8UI || internalformat == GL_R8I)
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
			GL_RED_INTEGER, internalformat == GL_R8UI ? GL_UNSIGNED_BYTE : GL_BYTE, nullptr);
	else
		glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
			GL_RGBA /* no debería importar */,
			GL_BYTE /* no debería importar */, NULL);
	CHECK_GL2("Error reservando memoria para la textura");

	_width = width;
	_height = height;
	_internalFormat = internalformat;
	_ready = true;
}

void Texture2DGeneric::setParams() {
	glTexParameteri(_texture_type, GL_TEXTURE_MAG_FILTER, _magfilter);
	glTexParameteri(_texture_type, GL_TEXTURE_MIN_FILTER, _minfilter);
	if (_texture_type != GL_TEXTURE_RECTANGLE) {
		glTexParameteri(_texture_type, GL_TEXTURE_WRAP_S, _wrap_s);
		glTexParameteri(_texture_type, GL_TEXTURE_WRAP_T, _wrap_t);
	}
}

void Texture2DGeneric::loadImageFromMemory(void *pixels, uint width,
	uint height, GLenum pixels_format, GLenum pixels_type,
	GLint internalformat) {
	_ready = false;
	/* Create and load textures to OpenGL */
	glBindTexture(_texture_type, this->_texId);
	setParams();

	glTexImage2D(_texture_type, 0, internalformat, width, height, 0,
		pixels_format, pixels_type, pixels);
	CHECK_GL();
	_width = width;
	_height = height;
	_internalFormat = internalformat;
	_ready = true;
}

void Texture2DGeneric::updateImageFromMemory(void *pixels, uint width, uint height, GLenum pixels_format,
	GLenum pixels_type) {

	glBindTexture(_texture_type, _texId);
	glTexSubImage2D(_texture_type, 0, 0, 0, width, height, pixels_format, pixels_type, pixels);

}


bool Texture2DGeneric::loadImage(const Image &image, GLenum internalFormat) {
	loadImageFromMemory(image.getPixels(), image.getWidth(), image.getHeight(),
		image.getGLFormatType(), image.getGLPixelBaseType(), internalFormat);
	return _ready;
}


bool Texture2DGeneric::loadImage(const Image &image) {
	return loadImage(image, image.getSuggestedGLInternalFormatType());
}

void Texture2DGeneric::updateImage(const Image &image) {
	updateImageFromMemory(image.getPixels(), image.getWidth(), image.getHeight(), image.getGLFormatType(), image.getGLPixelBaseType());
}


bool Texture2DGeneric::loadImage(const std::string &filename, GLenum internalFormat) {
	PGUPV::Image image(filename);
	_name = PGUPV::getFilenameFromPath(filename);
	return loadImage(image, internalFormat);
}

void saveBoundTexture(const std::string &filename, uint32_t width, uint32_t height, uint32_t bpp) {
	std::unique_ptr<uint8_t[]> bytes(new uint8_t[width * height * bpp / 8]);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	switch (bpp) {
	case 8:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, bytes.get());
		break;
	case 24:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes.get());
		break;
	case 32:
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.get());
		break;
	}

	PGUPV::Image image(width, height, bpp, bytes.get());
	image.save(filename);
}

void Texture2DGeneric::save(const std::string &filename, GLuint texId, unsigned int bpp) {
	if (bpp != 8 && bpp != 24 && bpp != 32) {
		ERRT("Sólo se puede guardar imágenes de 8, 24 y 32 bpp");
	}
	glActiveTexture(GL_TEXTURE0 + App::getScratchUnitTextureNumber());
	glBindTexture(GL_TEXTURE_2D, texId);
	GLint width, height;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	saveBoundTexture(filename, width, height, bpp);
}

void Texture2DGeneric::clear(int level, GLenum format, GLenum type, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const void * data)
{
	glClearTexSubImage(_texId, level, xoffset, yoffset, 0, width, height, 1, format, type, data);
}

void Texture2DGeneric::save(const std::string &filename, unsigned int bpp) {
	if (bpp != 8 && bpp != 24 && bpp != 32) {
		ERRT("Sólo se puede guardar imágenes de 8, 24 y 32 bpp");
	}
	bind(GL_TEXTURE0 + App::getScratchUnitTextureNumber());
	saveBoundTexture(filename, _width, _height, bpp);
}


void Texture2DGeneric::save(const std::string &filename, uint32_t bpp, 
	uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height) const {
	auto bufferSize = width * height * bpp / 8;
	std::unique_ptr<uint8_t[]> bytes(new uint8_t[bufferSize]);

	getPixels(xoffset, yoffset, width, height, bpp, &bytes[0]);

	PGUPV::Image image(width, height, bpp, &bytes[0]);
	image.save(filename);
}

void Texture2DGeneric::getPixels(uint32_t bpp, uint32_t xoffset, uint32_t yoffset, uint32_t width,
	uint32_t height, uint8_t *buffer) const {
	if (bpp != 8 && bpp != 24 && bpp != 32) {
		ERRT("Sólo se puede guardar imágenes de 8, 24 y 32 bpp");
	}
	if (xoffset + width > _width || yoffset + height > _height) {
		ERRT("La porción solicitada [" + std::to_string(xoffset) + ", " + std::to_string(yoffset) + ", w: " +
			std::to_string(width) + " h: " + std::to_string(height) + "] está fuera de la textura [w: " +
			std::to_string(_width) + " h: " + std::to_string(_height) + "]");
	}

	auto bufferSize = width * height * bpp / 8;
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	switch (bpp) {
	case 8:
		glGetTextureSubImage(_texId, 0, xoffset, yoffset, 0, width, height, 1, GL_RED, GL_UNSIGNED_BYTE, bufferSize, buffer);
		break;
	case 24:
		glGetTextureSubImage(_texId, 0, xoffset, yoffset, 0, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, bufferSize, buffer);
		break;
	case 32:
		glGetTextureSubImage(_texId, 0, xoffset, yoffset, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, bufferSize, buffer);
		break;
	}
}
