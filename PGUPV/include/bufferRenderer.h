#ifndef _BUFFER_RENDERER_H
#define _BUFFER_RENDERER_H 2013

#include <memory>

#include "program.h"
#include "fbo.h"
#include "textureRectangle.h"
#include "stockModels.h"
namespace PGUPV {

  class Window;

  /* Esta clase está asociada a una ventana, y se encarga de capturar, preparar y mostrar los
  buffers de profundidad y de stencil */
  class BufferRenderer {
  public:
    explicit BufferRenderer(Window &window);
    void showBuffer();
  private:
    // No permitir la copia
    BufferRenderer& operator=(BufferRenderer);
    Window &_window;
    Program _shader;
    // FBO para dibujar el z-buffer a uno de sus attachments de tipo textura. También se podría utilizar
    // para obtener el stencil, pero OpenGL 3.3 no da acceso dentro de los shaders a los datos del stencil
    // OpenGL 4.3 sí.
    FBO fbo;
    std::shared_ptr<TextureRectangle> depthStencilTex;
    std::unique_ptr<TextureRectangle> stencilTexture;
    void updateStencilTexture();
    void updateDepthTexture();
    GLint _showStencilLoc, _stencilOrDepthUnitLoc;
    ScreenPolygon screen;
  };

};

#endif
