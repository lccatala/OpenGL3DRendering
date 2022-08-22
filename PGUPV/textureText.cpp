
#include <assert.h>

#include "glStateCache.h"
#include "textureText.h"
#include "log.h"
#include "app.h"

#include <SDL_ttf.h>

using PGUPV::TextureTextBuilder;
using PGUPV::TextureText;
using PGUPV::Font;


TextureTextBuilder::TextureTextBuilder() {
  theFont = Font::getDefaultFont();
  theText = " ";
  theWrapWidth = 0;
  fgcolor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
}

TextureTextBuilder &TextureTextBuilder::font(std::shared_ptr<Font> font) {
  this->theFont = font;
  return *this;
}

TextureTextBuilder &TextureTextBuilder::text(const std::string &text) {
  this->theText = text;
  return *this;
}

TextureTextBuilder &TextureTextBuilder::wrapWidth(uint pixels) {
  this->theWrapWidth = pixels;
  return *this;
}

TextureTextBuilder &TextureTextBuilder::foregroundColor(const glm::vec4 &color) {
  this->fgcolor = color;
  return *this;
}

TextureTextBuilder &TextureTextBuilder::backgroundColor(const glm::vec4 &color) {
  this->bgcolor = color;
  return *this;
}

std::shared_ptr<TextureText> TextureTextBuilder::build() {
	
	PGUPV::GLStateCapturer< PGUPV::ActiveTextureUnitState> restoreActiveTextureUnit;
	glActiveTexture(GL_TEXTURE0 + PGUPV::App::getScratchUnitTextureNumber());
	auto result = std::shared_ptr<TextureText>(new TextureText(theText, theFont, theWrapWidth, fgcolor, bgcolor));
	result->setName(theText);
	return result;
}



TextureText::TextureText(const std::string &t, std::shared_ptr<Font> font, uint wrapWidth, const glm::vec4 &fgcolor, const glm::vec4 &bgcolor) :
Texture2D(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) {

  if (t.length() == 0) {
    ERRT("No se puede construir la textura de la cadena vacía");
  }

  SDL_Color sdlFGColor{ Uint8(fgcolor.r * 255), Uint8(fgcolor.g * 255), Uint8(fgcolor.b * 255), Uint8(fgcolor.a * 255) };
  SDL_Color sdlBGColor{ Uint8(bgcolor.r * 255), Uint8(bgcolor.g * 255), Uint8(bgcolor.b * 255), Uint8(bgcolor.a * 255) };
  SDL_Surface *textSurface;

  if (wrapWidth == 0) {
    textSurface = TTF_RenderUTF8_Shaded(font->getTTFFont(), t.c_str(), sdlFGColor, sdlBGColor);
  } else {
    textSurface = TTF_RenderUTF8_Blended_Wrapped(font->getTTFFont(), t.c_str(), sdlFGColor, wrapWidth);
  }

  
  if (textSurface == NULL)
    ERRT(std::string("No se ha podido generar el texto. Error SDL_ttf: ") + TTF_GetError());

#ifdef _DEBUG
  assert(textSurface->pitch % 4 == 0);
#endif

  uchar *temp;
  if (textSurface->format->palette) {
    temp = new uchar[textSurface->w * textSurface->h * 4];
    uchar *wtemp = temp, *rtemp;
    for (int i = 0; i < textSurface->h; i++) {
      rtemp = ((uchar *)textSurface->pixels) + (textSurface->h - i -1) * textSurface->pitch;
      for (int j = 0; j < textSurface->w; j++) {
        memcpy(wtemp, &textSurface->format->palette->colors[*rtemp].r, 4);
        wtemp += 4;
        rtemp++;
      }
    }
  }
  else {
    temp = new uchar[textSurface->pitch * textSurface->h];
    for (int i = 0; i < textSurface->h; i++) {
      memcpy(temp + textSurface->pitch * i,
        ((uchar *)textSurface->pixels) + (textSurface->h - 1 - i) * textSurface->pitch,
        textSurface->pitch);
    }
  }

  FRAME("Tamaño de la textura generada: " + std::to_string(textSurface->w) + ", " + std::to_string(textSurface->h));

/*  if (textSurface->format->BitsPerPixel == 8)
    loadImageFromMemory(temp, textSurface->w, textSurface->h, GL_RED, GL_UNSIGNED_BYTE, GL_RGBA);
  else if (textSurface->format->BitsPerPixel == 24) 
    loadImageFromMemory(temp, textSurface->w, textSurface->h, GL_RGB, GL_UNSIGNED_BYTE, GL_RGBA);
  else if (textSurface->format->BitsPerPixel == 32)*/

    loadImageFromMemory(temp, textSurface->w, textSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA);
  /*else
    ERRT("Formato de imagen no soportado");
    */

  delete[]temp;
  SDL_FreeSurface(textSurface);
}
