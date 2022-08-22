#ifndef _TEXTURETEXT_H
#define _TEXTURETEXT_H 2014

#include "font.h"
#include "texture2D.h"

namespace PGUPV {
  /** 
  \class TextureTextBuilder

  Construye una textura 2d que contiene el texto indicado. Ejemplo de uso:

  TextureTextBuilder tb;
  TextureText tt = tb.text("Hola mundo").font(Font::loadFont("arial.ttf", 20)).wrapWidth(50).build();

  (suponiendo que arial.ttf está en el directorio actual)

  Todos los parámetros son opcionales. 

  */
  class TextureText;

  class TextureTextBuilder {
  public:
    TextureTextBuilder();
    TextureTextBuilder &font(std::shared_ptr<Font> font);
    TextureTextBuilder &text(const std::string &text);
    TextureTextBuilder &wrapWidth(uint pixels);
    TextureTextBuilder &foregroundColor(const glm::vec4 &color);
    TextureTextBuilder &backgroundColor(const glm::vec4 &color);
    std::shared_ptr<TextureText> build();
  private:
    std::shared_ptr<Font> theFont;
    std::string theText;
    uint theWrapWidth;
    glm::vec4 fgcolor, bgcolor;
  };


  /** 
  Construye una textura (Texture2D) a partir de una cadena y una fuente de texto. Luego
  se puede utilizar la textura como cualquier otra.
  Usa la clase TextureTextBuilder para construir la textura
  */
  class TextureText : public PGUPV::Texture2D {
  public:
  virtual ~TextureText() {};
private:
  TextureText(const std::string &t, std::shared_ptr<Font> font, uint wrapWidth, const glm::vec4 &fgcolor, const glm::vec4 &bgcolor);

  friend TextureTextBuilder;



};

};
#endif
