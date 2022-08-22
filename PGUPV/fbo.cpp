
#include "fbo.h"
#include "log.h"
#include "texture3DGeneric.h"
#include "bindableTexture.h"

using PGUPV::FBO;
using PGUPV::RBO;
using PGUPV::BindableTexture;

GLint FBO::_maxColorAttachments = 0;

FBO::FBO() {
  if (_maxColorAttachments == 0) {
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_maxColorAttachments);
  }
  _colorAttachment.resize(_maxColorAttachments);
  _texColorAttachment.resize(_maxColorAttachments);
  glGenFramebuffers(1, &_fboId);

  INFO("Framebuffer object " + std::to_string(_fboId) + " creado");
}

FBO::FBO(FBO &&o) :
_fboId(std::move(o._fboId)),
_colorAttachment(std::move(o._colorAttachment)),
_depthAttachment(std::move(o._depthAttachment)),
_stencilAttachment(std::move(o._stencilAttachment)),
_depthStencilAttachment(std::move(o._depthStencilAttachment)),

_texColorAttachment(std::move(o._texColorAttachment)),
_texDepthAttachment(std::move(o._texDepthAttachment)),
_texStencilAttachment(std::move(o._texStencilAttachment)),
_texDepthStencilAttachment(std::move(o._texDepthStencilAttachment))
{
  o._fboId = 0;
}



FBO::~FBO() {
  if (_fboId) {
    glDeleteFramebuffers(1, &_fboId);
    INFO("Framebuffer object " + std::to_string(_fboId) + " destruído");
  }
}

GLuint FBO::bind(GLenum target) {
  GLint prevFBO;
  glGetIntegerv((target == GL_READ_FRAMEBUFFER ? GL_READ_FRAMEBUFFER_BINDING
    : GL_DRAW_FRAMEBUFFER_BINDING),
    &prevFBO);
  glBindFramebuffer(target, _fboId);
  return prevFBO;
}

void FBO::unbind(GLenum target) { glBindFramebuffer(target, 0); }

// Asocia el renderbuffer al attachment indicado (descartando el que hubiera
// previamente)
void FBO::attach(GLuint attachment, std::shared_ptr<RBO> rb) {
  GLuint prevFBO = bind(GL_DRAW_FRAMEBUFFER);
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    _depthAttachment = rb;
    break;
  case GL_STENCIL_ATTACHMENT:
    _stencilAttachment = rb;
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    _depthStencilAttachment = rb;
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      _colorAttachment[colorAtt] = rb;
    }
    else
      ERRT("Attachment no válido");
  }
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attachment, GL_RENDERBUFFER,
    rb->getId());
  if (prevFBO != _fboId) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
  }
}

// Asocia la textura al attachment indicado (la textura debe ser compatible y
// tener memoria asignada)
void FBO::attach(GLuint attachment, std::shared_ptr<BindableTexture> tx) {
  GLuint prevFBO = bind(GL_DRAW_FRAMEBUFFER);
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    _texDepthAttachment = tx;
    break;
  case GL_STENCIL_ATTACHMENT:
    _texStencilAttachment = tx;
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    _texDepthStencilAttachment = tx;
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      _texColorAttachment[colorAtt] = tx;
    }
    else
      ERRT("Attachment no válido");
  }
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, tx->getTextureType(),
    tx->getId(), 0);
  if (prevFBO != _fboId) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
  }
}

void FBO::attach(GLuint attachment, std::shared_ptr<PGUPV::Texture3DGeneric> tx, uint layer) {
  GLuint prevFBO = bind(GL_DRAW_FRAMEBUFFER);
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    _texDepthAttachment = tx;
    break;
  case GL_STENCIL_ATTACHMENT:
    _texStencilAttachment = tx;
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    _texDepthStencilAttachment = tx;
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      _texColorAttachment[colorAtt] = tx;
    }
    else
      ERRT("Attachment no válido");
  }
  glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attachment,
    tx->getId(), 0, layer);
  if (prevFBO != _fboId) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);
  }
}

// Crea un renderbuffer y lo asocia al attachment indicado
void FBO::createAndAttach(GLuint attachment, uint width, uint height, uint samples) {
  PGUPV::RBOFactory rboF;

  rboF.width(width).height(height).samples(samples);
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    rboF.internalFormat(GL_DEPTH_COMPONENT);
    break;
  case GL_STENCIL_ATTACHMENT:
    rboF.internalFormat(GL_STENCIL_INDEX);
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    rboF.internalFormat(GL_DEPTH_STENCIL);
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      rboF.internalFormat(GL_RGBA);
    }
    else
      ERRT("Attachment no válido");
  }
  attach(attachment, rboF.build());
}

// Devuelve el RBO que está vinculando al attachment indicado
std::shared_ptr<RBO> FBO::getAttachedRBO(GLuint attachment) const {
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    return _depthAttachment;
    break;
  case GL_STENCIL_ATTACHMENT:
    return _stencilAttachment;
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    return _depthStencilAttachment;
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      return _colorAttachment[colorAtt];
    }
  }
  ERRT("Attachment no válido");
}

std::shared_ptr<BindableTexture>
FBO::getAttachedTexture(GLuint attachment) const {
  switch (attachment) {
  case GL_DEPTH_ATTACHMENT:
    return _texDepthAttachment;
    break;
  case GL_STENCIL_ATTACHMENT:
    return _texStencilAttachment;
    break;
  case GL_DEPTH_STENCIL_ATTACHMENT:
    return _texDepthStencilAttachment;
    break;
  default:
    long colorAtt = (long)attachment - GL_COLOR_ATTACHMENT0;
    if (colorAtt >= 0 && colorAtt < _maxColorAttachments) {
      return _texColorAttachment[colorAtt];
    }
  }
  ERRT("Attachment no válido");
}

bool FBO::isComplete() {
  GLint prevFBO = bind(GL_DRAW_FRAMEBUFFER);
  GLenum error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFBO);

  if (error != GL_FRAMEBUFFER_COMPLETE) {
    std::string message = "FBO incompleto: ";
    switch (error) {
#define CE(m)                                                                  \
  case m:                                                                      \
    message += #m;                                                             \
    break
      CE(GL_FRAMEBUFFER_UNDEFINED);
      CE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
      CE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
      CE(GL_FRAMEBUFFER_UNSUPPORTED);
      CE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
      CE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
#undef CE
  default:
    message += "Error desconocido: " + hexString(error);
    }
    ERR(message);
    return false;
  }

  return true;
}

bool PGUPV::checkFBOCompleteness(GLenum bindLocation, std::string &message) {
  GLenum error;
  if ((error = glCheckFramebufferStatus(bindLocation)) ==
    GL_FRAMEBUFFER_COMPLETE) {
    message = "FBO completo";
    return true;
  }
  else {
    message = "FBO incompleto: ";

#define CE(m)                                                                  \
  case m:                                                                      \
    message += #m;                                                             \
    break

    switch (error) {
      CE(GL_FRAMEBUFFER_UNDEFINED);
      CE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
      CE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
      CE(GL_FRAMEBUFFER_UNSUPPORTED);
      CE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
      CE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);

#undef CE
    }
    return false;
  }
}
