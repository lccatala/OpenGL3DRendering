#include "viewportRenderer.h"

using PGUPV::ViewportRenderer;

void ViewportRenderer::preRender() {
  viewport.capture();
  scissor.capture();

  glEnable(GL_SCISSOR_TEST);
  glViewport(origin.x, origin.y, size.x, size.y);
  glScissor(origin.x, origin.y, size.x, size.y);
}

void ViewportRenderer::postRender() {
  scissor.restore();
  viewport.restore();
}

bool ViewportRenderer::isInViewport(uint x, uint y)
{
  return x >= origin.x && y >= origin.y && x <= (origin.x + size.x) && y <= (origin.y + size.y);
}
