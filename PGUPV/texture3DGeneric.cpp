#include "texture3DGeneric.h"
#include "utils.h"
#include "log.h"
#include "image.h"

using PGUPV::Texture3DGeneric;
using PGUPV::Image;

Texture3DGeneric::Texture3DGeneric(
  GLenum texture_type, GLenum minfilter, GLenum magfilter, GLenum wrap_s, GLenum wrap_t, GLenum wrap_r) :
  Texture(texture_type, minfilter, magfilter, wrap_s, wrap_t, wrap_r), _width(0), _height(0)
{
  if (texture_type != GL_TEXTURE_3D && texture_type != GL_PROXY_TEXTURE_3D &&
    texture_type != GL_TEXTURE_2D_ARRAY  && texture_type != GL_PROXY_TEXTURE_2D_ARRAY)
    ERRT("Esta clase no soporta le tipo de textura pedido: " + PGUPV::hexString(texture_type));
};

// Allocates memory for a texture with the given size and format
void Texture3DGeneric::allocate(uint width, uint height, uint depth, GLint internalformat) {
  glBindTexture(_texture_type, _texId);
  setParams();
  if (internalformat == GL_RED || internalformat == GL_RG || internalformat == GL_RGB
    || internalformat == GL_RGBA)
    glTexImage3D(_texture_type, 0, internalformat, width, height, depth,
    0, internalformat, GL_UNSIGNED_BYTE, NULL);
  else if (internalformat == GL_DEPTH_COMPONENT || internalformat == GL_DEPTH_COMPONENT16 ||
    internalformat == GL_DEPTH_COMPONENT24 || internalformat == GL_DEPTH_COMPONENT32)
    glTexImage3D(_texture_type, 0, internalformat, width, height, depth,
    0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  else if (internalformat == GL_DEPTH_STENCIL)
    glTexImage3D(_texture_type, 0, GL_DEPTH_STENCIL, width, height, depth, 0,
    GL_DEPTH_STENCIL /* no debería importar */, GL_UNSIGNED_INT_24_8 /* no debería importar */, NULL);
  else
    glTexImage3D(_texture_type, 0, internalformat, width, height, depth, 0,
    GL_RGBA /* no debería importar */, GL_BYTE /* no debería importar */, NULL);
  CHECK_GL2("Error reservando memoria para la textura");

  _width = width;
  _height = height;
  _depth = depth;
  _ready = true;
}

void Texture3DGeneric::setParams() {
  glTexParameteri(_texture_type, GL_TEXTURE_MAG_FILTER, _magfilter);
  glTexParameteri(_texture_type, GL_TEXTURE_MIN_FILTER, _minfilter);
  glTexParameteri(_texture_type, GL_TEXTURE_WRAP_S, _wrap_s);
  glTexParameteri(_texture_type, GL_TEXTURE_WRAP_T, _wrap_t);
  if (_texture_type == GL_TEXTURE_3D) {
    glTexParameteri(_texture_type, GL_TEXTURE_WRAP_R, _wrap_r);
  }
}


void Texture3DGeneric::loadSlice(void *pixels, uint width, uint height, uint slice, GLenum pixels_format,
  GLenum pixels_type, GLint /*internalformat*/) {
  if (width != _width || height != _height)
    ERRT("Antes de cargar un frame hay que reservar espacio con allocate()");
  if (slice >= _depth)
    ERRT("Intentando añadir más frames de los reservados con allocate()");

  _ready = false;
  /* Create and load textures to OpenGL */
  glBindTexture(_texture_type, _texId);
  setParams();

  glTexSubImage3D(_texture_type, 0, 0, 0, slice, width, height, 1, pixels_format, pixels_type,
    pixels);
  CHECK_GL();
  _ready = true;
}

void Texture3DGeneric::loadSlice(const PGUPV::Image &image, uint slice) {
  if (image.getAnimationFrames() > 1)
    WARN("Sólo se cargará el primer frame de la imagen");
  loadSlice(image.getPixels(), image.getWidth(), image.getHeight(), slice, image.getGLFormatType(),
    image.getGLPixelBaseType(), image.getSuggestedGLInternalFormatType());
}

void Texture3DGeneric::loadImage(const Image &image) {

    GLenum format = image.getSuggestedGLInternalFormatType();

  allocate(image.getWidth(), image.getHeight(), image.getAnimationFrames(), format);
  for (uint i = 0; i < image.getAnimationFrames(); i++) {
    loadSlice(image.getPixels(0, 0, i), image.getWidth(), image.getHeight(), i, image.getGLFormatType(), image.getGLPixelBaseType(), format);
  }
}

bool Texture3DGeneric::loadImage(const std::string &filename) {
  PGUPV::Image image(filename);
  loadImage(image);
  return _ready;
}
