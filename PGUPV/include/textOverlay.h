#pragma once

#include "renderer.h"

namespace PGUPV {
  class Rect;
  class TextureText;
  class GLMatrices;
  class Program;

  /**
  \class TextOverlay
  Clase que muestra una caja de texto
  */

  class TextOverlay : public Renderer {
  public:
    TextOverlay(const std::string &text);
    void setText(const std::string &text);
    void setup() override;
    void render() override;
    void reshape(uint w, uint h) override;
    void setMaxWidth(uint w);
  private:
    std::shared_ptr<Rect> overlay, background;
    std::shared_ptr<TextureText> textHelp;
    std::shared_ptr<GLMatrices> mats;
    unsigned int windowWidth, windowHeight;
    unsigned int maxWidth;
    unsigned int textWidth, textHeight;
  };
};
