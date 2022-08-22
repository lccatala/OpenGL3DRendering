

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "app.h"
#include "textOverlay.h"
#include "indexedBindingPoint.h"
#include "glMatrices.h"
#include "glStateCache.h"
#include "textureText.h"
#include "stockModels.h"
#include "stockPrograms.h"
#include "program.h"

using PGUPV::TextOverlay;


TextOverlay::TextOverlay(const std::string &text) : maxWidth(0) {
  setText(text);
}

void TextOverlay::setText(const std::string &text) {
  PGUPV::TextureTextBuilder tb;
  textHelp = tb.text(text).font(Font::loadFont("../recursos/fuentes/FreeSans.ttf", 14)).wrapWidth(400).
    foregroundColor(glm::vec4(.9f, .9f, .9f, 1.0f)).build();
  float invAspectRatio = float(textHelp->getHeight()) / textHelp->getWidth();
  textWidth = textHelp->getWidth();
  textHeight = textHelp->getHeight();
  overlay = std::unique_ptr<Rect>(new Rect(1.0f, invAspectRatio));
  background = std::unique_ptr<Rect>(new Rect(1.1f, 1.1f * invAspectRatio, glm::vec4(.2f, .2f, .2f, 0.7f)));

}

void TextOverlay::setMaxWidth(unsigned int maxW) {
  maxWidth = maxW;
}

void TextOverlay::setup() {
  mats = GLMatrices::build();
  mats->setMatrix(GLMatrices::PROJ_MATRIX, glm::ortho(-0.6f, 0.6f, -0.6f, 0.6f));
}

void TextOverlay::render() {
  PGUPV::GLStateCapturer<PGUPV::BlendingState> blendSt;
  PGUPV::GLStateCapturer<PGUPV::ViewportState> viewport;
  PGUPV::GLStateCapturer<PGUPV::PolygonModeState> polygonMode;
  PGUPV::ColorMasksState colorMasks;
  PGUPV::DepthTestState depthTest;

  // Salvamos el valor actual del atributo genérico del color
  PGUPV::GenericAttribState attrib(std::vector<GLint> {2});

  if (textWidth * windowHeight < textHeight * windowWidth) {
    // text's aspect ratio < window's aspect ratio
    uint ew = windowHeight * textWidth / textHeight;
    glViewport((windowWidth - ew) / 2, 0, ew, windowHeight);
  }
  else {
    uint eh = windowWidth * textHeight / textWidth;
    glViewport(0, (windowHeight - eh) / 2, windowWidth, eh);
  }

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  auto bo = gl_uniform_buffer.bindBufferBase(mats, UBO_GL_MATRICES_BINDING_INDEX);
  
  auto prevProg = PGUPV::ConstantIllumProgram::use();
  background->render();

  PGUPV::TextureReplaceProgram::use();
  int prevTU = PGUPV::TextureReplaceProgram::setTextureUnit(App::getScratchUnitTextureNumber());
  textHelp->bind(GL_TEXTURE0 + App::getScratchUnitTextureNumber());

  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.0f, 0.0f, 0.0001f);
  overlay->render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  PGUPV::TextureReplaceProgram::setTextureUnit(prevTU);

  if (prevProg != nullptr) 
    prevProg->use();
  gl_uniform_buffer.bind(bo);

  polygonMode.restore();
  attrib.restore();
  depthTest.restore();
  colorMasks.restore();
  viewport.restore();
  blendSt.restore();
}

void TextOverlay::reshape(uint w, uint h) {
  windowWidth = w;
  windowHeight = h;
}