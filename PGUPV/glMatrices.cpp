#include <glm/glm.hpp>

#include "glMatrices.h"
#include "log.h"
#include "indexedBindingPoint.h"
#include "utils.h"

using PGUPV::GLMatrices;
using PGUPV::BufferObject;

GLMatrices::GLMatrices() : UniformBufferObject(size()) {
	derivedMatrices.modelview = glm::mat4(1.0f);
	derivedMatrices.modelviewprojection = glm::mat4(1.0f);
	derivedMatrices.normal = glm::mat3x4(1.0f);
}

const std::string GLMatrices::blockName{ "GLMatrices" };

const std::string &GLMatrices::getBlockName() const { 
	return GLMatrices::blockName; 
}

uint GLMatrices::size() { return 5 * sizeof(glm::mat4) + sizeof(glm::mat3x4); }

const Strings GLMatrices::definition{
"layout (std140, binding=" + std::to_string(UBO_GL_MATRICES_BINDING_INDEX) + ") uniform GLMatrices {",
"  mat4 modelMatrix;",
"  mat4 viewMatrix;",
"  mat4 projMatrix;",
"  mat4 modelviewMatrix;",
"  mat4 modelviewprojMatrix;",
"  mat3 normalMatrix;",
"};",
};

const Strings &GLMatrices::getDefinition() const {
	return GLMatrices::definition;
}

void GLMatrices::writeMatrix(Matrix mat) {
  gl_uniform_buffer.bindBufferBase(this->shared_from_this(),
    UBO_GL_MATRICES_BINDING_INDEX);
  gl_uniform_buffer.write((void *)&mats[mat].getMatrix(), sizeof(glm::mat4),
    mat * sizeof(glm::mat4));
  if (mat == MODEL_MATRIX || mat == VIEW_MATRIX) {
    // Compute MODELVIEW matrix
    derivedMatrices.modelview =
      mats[VIEW_MATRIX].getMatrix() * mats[MODEL_MATRIX].getMatrix();

    // Compute MODELVIEWPROJ matrix
    derivedMatrices.modelviewprojection =
      mats[PROJ_MATRIX].getMatrix() * derivedMatrices.modelview;

    // Compute NORMAL matrix
    glm::mat3 nm(derivedMatrices.modelview);
    derivedMatrices.normal = glm::mat3x4(glm::transpose(glm::inverse(nm)));

    // Write both into the UBO
    gl_uniform_buffer.write((void *)&derivedMatrices, sizeof(derivedMatrices),
      sizeof(glm::mat4) * 3);
  }
  else if (mat == PROJ_MATRIX) {
    // Compute MODELVIEWPROJ matrix
    derivedMatrices.modelviewprojection =
      mats[PROJ_MATRIX].getMatrix() * derivedMatrices.modelview;
    // Write both into the UBO
    gl_uniform_buffer.write((void *)&derivedMatrices, sizeof(derivedMatrices),
      sizeof(glm::mat4) * 3);
  }
}

void GLMatrices::loadIdentity(Matrix mat) {
  mats[mat].loadIdentity();
  writeMatrix(mat);
}

void GLMatrices::pushMatrix(Matrix mat) {
  mats[mat].pushMatrix();
  // This function does not change the value of the uniform
}

void GLMatrices::popMatrix(Matrix mat) {
  mats[mat].popMatrix();
  writeMatrix(mat);
}

void GLMatrices::translate(Matrix mat, float x, float y, float z) {
  translate(mat, glm::vec3(x, y, z));
}

void GLMatrices::translate(Matrix mat, const glm::vec3 &t) {
  mats[mat].translate(t);
  writeMatrix(mat);
}

void GLMatrices::rotate(Matrix mat, float radians, float axis_x, float axis_y,
  float axis_z) {
  rotate(mat, radians, glm::vec3(axis_x, axis_y, axis_z));
}

void GLMatrices::rotate(Matrix mat, float radians, const glm::vec3 &axis) {
  mats[mat].rotate(radians, axis);
  writeMatrix(mat);
}


void GLMatrices::scale(Matrix mat, float s) {
    scale(mat, glm::vec3(s));
}

void GLMatrices::scale(Matrix mat, float sx, float sy, float sz) {
  scale(mat, glm::vec3(sx, sy, sz));
}

void GLMatrices::scale(Matrix mat, const glm::vec3 &s) {
  mats[mat].scale(s);
  writeMatrix(mat);
}

void GLMatrices::multMatrix(Matrix mat, const glm::mat4 &m) {
  mats[mat].multMatrix(m);
  writeMatrix(mat);
}

void GLMatrices::setMatrix(Matrix mat, const glm::mat4 &m) {
  mats[mat].setMatrix(m);
  writeMatrix(mat);
}

const glm::mat4 &GLMatrices::getMatrix(Matrix mat) const {
  if (mat == NORMAL_MATRIX)
    ERRT("No puedes usar getMatrix para conseguir la matriz normal. Usa "
    "getNormalMatrix");

  if (mat == MODELVIEW_MATRIX)
    return derivedMatrices.modelview;
  else if (mat == MODELVIEWPROJ_MATRIX)
    return derivedMatrices.modelviewprojection;
  else
    return mats[mat].getMatrix();
}

const glm::mat3 GLMatrices::getNormalMatrix() const {
  return glm::mat3(derivedMatrices.normal);
}

void GLMatrices::reset() {
  for (uint i = 0; i <= PROJ_MATRIX; i++) {
    mats[i].reset();
    writeMatrix((Matrix)i);
  }
}

std::shared_ptr<GLMatrices> GLMatrices::build() {
  std::shared_ptr<GLMatrices> ubo =
    std::shared_ptr<GLMatrices>(new GLMatrices());
  allocate(ubo);
  ubo->setGlDebugLabel(ubo->getBlockName());
  ubo->reset();
  INFO("UBO GLMatrices creado");
  return ubo;
}

std::ostream &PGUPV::operator<<(std::ostream &os, const GLMatrices &m) {
  os << "Model matrix:\n";
  os << m.mats[GLMatrices::MODEL_MATRIX];
  os << "View matrix:\n";
  os << m.mats[GLMatrices::VIEW_MATRIX];
  os << "Projection matrix:\n";
  os << m.mats[GLMatrices::PROJ_MATRIX];
  os << "Model view matrix:\n";
  os << PGUPV::to_string(m.derivedMatrices.modelview);
  os << "Model view projection matrix:\n";
  os << PGUPV::to_string(m.derivedMatrices.modelviewprojection);
  os << "Normal matrix:\n";
  os << PGUPV::to_string(glm::mat3(m.derivedMatrices.normal));
  return os;
}
