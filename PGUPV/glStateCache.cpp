#include "glStateCache.h"
#include "hacks.h"
#include "log.h"


using PGUPV::GLStateCapturer;
using PGUPV::BlendingState;
using PGUPV::ViewportState;
using PGUPV::GenericAttribState;
using PGUPV::PolygonModeState;
using PGUPV::ColorMasksState;
using PGUPV::ScissorTestState;


void BlendingState::capture() {
  glGetIntegerv(GL_BLEND, &blending);
  glGetFloatv(GL_BLEND_COLOR, &blendColor.r);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
  glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcRGB);
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &eqAlpha);
  glGetIntegerv(GL_BLEND_EQUATION_RGB, &eqRGB);
}

void BlendingState::restore() {
  if (blending)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
  glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
  glBlendEquationSeparate(eqRGB, eqAlpha);
  glBlendColor(blendColor.r, blendColor.g, blendColor.b, blendColor.a);
}


void ViewportState::capture() {
  glGetIntegerv(GL_VIEWPORT, view);
}

void ViewportState::restore() {
  glViewport(view[0], view[1], view[2], view[3]);
}


void PolygonModeState::capture() {
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
}

void PolygonModeState::restore() {
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
}


GenericAttribState::GenericAttribState(const std::vector<GLint> &attribIds) {
  for (auto id : attribIds) {
    glm::vec4 v;
    glGetVertexAttribfv(id, GL_CURRENT_VERTEX_ATTRIB, &v.x);
    attribs.push_back(std::pair<GLint, glm::vec4>(id, v));
  }
}


void GenericAttribState::restore() {
  for (auto p : attribs) {
    glVertexAttrib4fv(p.first, &p.second.x);
  }
}

ColorMasksState::ColorMasksState() {
  glGetBooleanv(GL_COLOR_WRITEMASK, mask);
}

void ColorMasksState::restore() {
  glColorMask(mask[0], mask[1], mask[2], mask[3]);
}

PGUPV::DepthTestState::DepthTestState()
{
  glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
}

void PGUPV::DepthTestState::restore()
{
  if (depthTestEnabled)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

PGUPV::StencilTestEnabledState::StencilTestEnabledState() {
  stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
}

void PGUPV::StencilTestEnabledState::restore() {
  if (stencilTestEnabled)
    glEnable(GL_STENCIL_TEST);
  else
    glDisable(GL_STENCIL_TEST);
}

PGUPV::CurrentProgramState::CurrentProgramState() {
  glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
}

void PGUPV::CurrentProgramState::restore() {
  glUseProgram(prevProgram);
}


PGUPV::StencilMaskState::StencilMaskState() {
  glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilWriteMask);
}

void PGUPV::StencilMaskState::restore() {
  glStencilMask(stencilWriteMask);
}

void ScissorTestState::capture() {
  enabled = glIsEnabled(GL_SCISSOR_TEST);
  glGetIntegerv(GL_SCISSOR_BOX, scissorReg);
}

void ScissorTestState::restore() {
  if (enabled) 
    glEnable(GL_SCISSOR_TEST);
  else 
    glDisable(GL_SCISSOR_TEST);
  glScissor(scissorReg[0], scissorReg[1], scissorReg[2], scissorReg[3]);
}



