#ifndef _FBO_H
#define _FBO_H 2013

#include <GL/glew.h>
#include <memory>

#include "utils.h"
#include "rbo.h"

namespace PGUPV {

class BindableTexture;
class Texture3DGeneric;

class FBO {
public:
  FBO();
  ~FBO();
  /** 
  Constructor move
  */
  FBO(FBO &&);
  // Vincula el FBO para lectura (GL_READ_FRAMEBUFFER), escritura
  // (GL_DRAW_FRAMEBUFFER),
  // o ambos (GL_FRAMEBUFFER). Devuelve el id del FBO que estaba vinculado
  // previamente
  GLuint bind(GLenum target = GL_DRAW_FRAMEBUFFER);
  // Desvincula el FBO del punto de vinculación indicado, vinculando el
  // framebuffer por defecto
  void unbind(GLenum target = GL_FRAMEBUFFER);
  // Asocia el renderbuffer al attachment indicado (descartando el que hubiera
  // previamente)
  void attach(GLuint attachment, std::shared_ptr<RBO> rb);
  /**
   Crea un renderbuffer con las características indicadas y lo asocia al
   attachment indicado
   \param attachment Punto de vinculación del renderbuffer
   (GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, etc.)
   \param width Ancho del renderbuffer
   \param height Alto del renderbuffer
   \param samples Número de muestras del renderbuffer. Por defecto, cero
   */
  void createAndAttach(GLuint attachment, uint width, uint height, uint samples = 0);
  /** Asocia la textura al attachment indicado (la textura debe ser compatible y
   tener memoria asignada)
   \param attachment Punto de vinculación de la textura (GL_COLOR_ATTACHMENT0,
   GL_DEPTH_ATTACHMENT, etc.)
   \param tx La textura a vincular
   */
  void attach(GLuint attachment, std::shared_ptr<BindableTexture> tx);
  /** Asocia una capa de la textura 3D al attachment indicado (la textura debe
   ser compatible y tener memoria asignada)
   \param attachment Punto de vinculación de la textura (GL_COLOR_ATTACHMENT0,
   GL_DEPTH_ATTACHMENT, etc.)
   \param tx La textura a vincular
   \param layer Capa de la textura a vincular
   */
  void attach(GLuint attachment, std::shared_ptr<PGUPV::Texture3DGeneric> tx,
              uint layer);
  /**
   Devuelve el RBO que está vinculando al attachment indicado, si lo hay
   */
  std::shared_ptr<RBO> getAttachedRBO(GLuint attachment) const;
  /**
   Devuelve la textura vinculada al attachment indicado, si la hay
   */
  std::shared_ptr<BindableTexture> getAttachedTexture(GLuint attachment) const;
  /**
   Devuelve si el FBO está completo (si está listo para usarse)
   */
  bool isComplete();

private:
  // Prohibir la copia
  FBO(const FBO &other);
  FBO & operator=(const FBO &);

  GLuint _fboId;

  std::vector<std::shared_ptr<RBO>> _colorAttachment;
  std::shared_ptr<RBO> _depthAttachment;
  std::shared_ptr<RBO> _stencilAttachment;
  std::shared_ptr<RBO> _depthStencilAttachment;

  std::vector<std::shared_ptr<BindableTexture>> _texColorAttachment;
  std::shared_ptr<BindableTexture> _texDepthAttachment;
  std::shared_ptr<BindableTexture> _texStencilAttachment;
  std::shared_ptr<BindableTexture> _texDepthStencilAttachment;

  static GLint _maxColorAttachments;
};

/**
Devuelve si el FBO asociado al punto de anclaje está completo o no, y
proporciona información en la cadena
\param bindLocation punto de vinculación
\param message parámetro de salida donde se escribirá la información asociada a
la comprobación
\return true si el FBO está completo
*/
bool checkFBOCompleteness(GLenum bindLocation, std::string &message);
};

#endif
