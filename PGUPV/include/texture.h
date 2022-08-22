
#ifndef _TEXTURE_H
#define _TEXTURE_H 2011

#include <GL/glew.h>
#include <bindableTexture.h>
#include <glm/glm.hpp>

namespace PGUPV {

/* Esta clase representa la parte común de todos los tipos de texturas
en OpenGL. Permite cargar establecer los parámetros para su uso.
*/
class Texture : public BindableTexture {
public:
  // Constructor
  Texture(GLenum texture_type, GLenum minfilter, GLenum magfilter,
          GLenum wrap_s, GLenum wrap_t = GL_REPEAT, GLenum wrap_r = GL_REPEAT,
          const glm::vec4 &bordercolor = glm::vec4(0.0, 0.0, 0.0, 0.0));
  explicit Texture(Texture &&other) = default;

  virtual ~Texture(){};

  // Establece los filtros de minimización/maximización
  void setMinFilter(GLenum filter);
  void setMagFilter(GLenum filter);
  // Establece los modos de repetición
  void setWrapS(GLenum wrap);
  void setWrapT(GLenum wrap);
  void setWrapR(GLenum wrap);
  /**
   Establece el modo de comparación de la textura (GL_TEXTURE_COMPARE_MODE)
   \param compareMode Modo de comparación (GL_NONE o GL_COMPARE_REF_TO_TEXTURE)
   */
  void setCompareMode(GLenum compareMode);
  /**
   Establece el operador de comparación de la textura (GL_TEXTURE_COMPARE_FUNC)
  \param compareFunc El operador a utilizar (GL_LEQUAL, GL_GEQUAL, etc.)
   */
  void setCompareFunc(GLenum compareFunc);
  /**
  Establece el color del borde de la textura que se usa en el modo
  GL_CLAMP_TO_BORDER
  \param color color del borde
  */
  void setBorderColor(const glm::vec4 &color);

  /**
  Pedir a OpenGL que genere los mipmaps de la textura (glGenerateMipmaps)
  */

  void generateMipmap();

  // Funciones de consulta.
  GLenum getMinFilter() const { return _minfilter; };
  GLenum getMagFilter() const { return _magfilter; };
  GLenum getWrapS() const { return _wrap_s; };
  GLenum getWrapT() const { return _wrap_t; };
  GLenum getWrapR() const { return _wrap_r; };
  glm::vec4 getBorderColor() const { return _bordercolor; };

  /**
   Devuelve el modo de comparación de la textura (GL_TEXTURE_COMPARE_MODE)
   \returns GL_NONE o GL_COMPARE_REF_TO_TEXTURE
   */
  GLenum getCompareMode(void) const { return _compareMode; };
  /**
   Devuelve el operador de comparación de la textura (GL_TEXTURE_COMPARE_FUNC)
   \returns GL_LEQUAL, GL_EQUAL...
   */
  GLenum getCompareFunc(void) const { return _compareFunc; };

  GLenum getInternalFormat() const { return _internalFormat; }
protected:
  void setTexParam(GLenum pname, GLint value);
  void setTexParam(GLenum pname, const GLfloat *value);
  GLenum _minfilter, _magfilter, _wrap_s, _wrap_t, _wrap_r, _compareMode,
      _compareFunc, _internalFormat;
  glm::vec4 _bordercolor;
};
};

#endif
