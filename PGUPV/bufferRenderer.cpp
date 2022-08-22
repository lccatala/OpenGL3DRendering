
#include <glm/glm.hpp>

#include "app.h"
#include "bufferRenderer.h"
#include "window.h"
#include "colorWheel.h"
#include "log.h"
#include "glStateCache.h"

using namespace PGUPV;

BufferRenderer::BufferRenderer(Window &window)
    : _window(window) {
  _shader.addAttributeLocation(Mesh::VERTICES, "position");
  _shader.loadFiles("../recursos/shaders/showDepthStencil");
  _shader.compile();
  _showStencilLoc = _shader.getUniformLocation("showStencil");
  _stencilOrDepthUnitLoc = _shader.getUniformLocation("stencilOrDepthUnit");
}

void BufferRenderer::updateDepthTexture() {
  // Prepare the texture
  BindableTexture *ot =
      fbo.getAttachedTexture(GL_DEPTH_STENCIL_ATTACHMENT).get();
  TextureRectangle *otr = dynamic_cast<TextureRectangle *>(ot);

  if (otr == nullptr || otr->getHeight() != _window.height() ||
      otr->getWidth() != _window.width()) {
    depthStencilTex = std::make_shared<TextureRectangle>();
    depthStencilTex->allocate(_window.width(), _window.height(),
                              GL_DEPTH_STENCIL);
    fbo.attach(GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTex);
    CHECK_GL();
    if (!fbo.isComplete()) {
        ERRT("No se ha podido configurar completamente el FBO.");
    }
  }

  GLint prevDrawFB, prevReadFB;
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFB);
  prevDrawFB = fbo.bind(GL_DRAW_FRAMEBUFFER);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, prevDrawFB);
  glBlitFramebuffer(0, 0, _window.width() - 1, _window.height() - 1, 0, 0,
                    _window.width() - 1, _window.height() - 1,
                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFB);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFB);
}

void BufferRenderer::showBuffer() {
  if (_window.getShownBuffer() == Window::COLOR_BUFFER)
    return;

  // Disable the stencil test
  StencilTestEnabledState stencilTestState;
  glDisable(GL_STENCIL_TEST);

  // Save the active texture unit
  GLint prevActiveTexture;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveTexture);
  // Save the current Draw and Read Buffers
  GLint prevDrawBuffer, prevReadBuffer;
  glGetIntegerv(GL_DRAW_BUFFER, &prevDrawBuffer);
  glGetIntegerv(GL_READ_BUFFER, &prevReadBuffer);

  // Install our program and set our texture
  auto prevShader = _shader.use();
  glUniform1i(_stencilOrDepthUnitLoc, App::getScratchUnitTextureNumber());
  glActiveTexture(GL_TEXTURE0 + App::getScratchUnitTextureNumber());

  switch (_window.getShownBuffer()) {
  case Window::STENCIL_BUFFER:
    updateStencilTexture();
    glUniform1i(_showStencilLoc, 1);
    glBindTexture(stencilTexture->getTextureType(), stencilTexture->getId());
    break;
  case Window::DEPTH_BUFFER:
    updateDepthTexture();
    glUniform1i(_showStencilLoc, 0);
    glBindTexture(depthStencilTex->getTextureType(), depthStencilTex->getId());
    break;
  default:
    break;
  }

  glDrawBuffer(GL_BACK);

  DepthTestState depthState;
  glDisable(GL_DEPTH_TEST);
  screen.render();
  depthState.restore();

  // Restore the previous state
  if (prevShader)
      prevShader->use();
  glActiveTexture(prevActiveTexture);
  glDrawBuffer(prevDrawBuffer);
  glReadBuffer(prevReadBuffer);

  stencilTestState.restore();
}

void BufferRenderer::updateStencilTexture() {
  glReadBuffer(GL_BACK);

  unsigned char *colorCodedStencil =
      new unsigned char[_window.width() * _window.height() * 4];
  unsigned char *stencil =
      new unsigned char[_window.width() * _window.height()];
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glReadPixels(0, 0, _window.width(), _window.height(), GL_STENCIL_INDEX,
               GL_UNSIGNED_BYTE, stencil);

  unsigned char *s = stencil;
  unsigned char *p = colorCodedStencil;
  for (ulong i = 0; i < _window.width() * _window.height(); i++) {
    uchar *color = PGUPV::ColorWheel::getColor(*s++);
    for (int j = 0; j < 4; j++)
      *p++ = *color++;
  }
  delete[] stencil;

  if (stencilTexture &&
      (stencilTexture->getWidth() != _window.width() ||
       stencilTexture->getHeight() != _window.height())) {
    
    stencilTexture.reset();
  }

  if (!stencilTexture) {
    stencilTexture = std::unique_ptr<TextureRectangle>(new TextureRectangle());
    stencilTexture->allocate(_window.width(), _window.height(), GL_RGBA);
  }

  glBindTexture(stencilTexture->getTextureType(), stencilTexture->getId());

  glTexSubImage2D(stencilTexture->getTextureType(), 0, 0, 0, _window.width(),
                  _window.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                  colorCodedStencil);

  delete[] colorCodedStencil;
}
