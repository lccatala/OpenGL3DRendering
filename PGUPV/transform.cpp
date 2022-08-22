#include "transform.h"
#include "nodeVisitor.h"
#include "indexedBindingPoint.h"

using PGUPV::Transform;
using PGUPV::NodeVisitor;
using PGUPV::Value;

std::shared_ptr<Transform> Transform::build(const glm::mat4 & xform) {
  auto ret = std::shared_ptr<Transform>(new Transform(xform));
  return ret;
}

void Transform::setTransform(const glm::mat4 &xform) {
  transf.setValue(xform);
  invalidateBoundingVolumes();
}

glm::mat4 Transform::getTransform() const {
  return transf.getValue();
}

Value<glm::mat4> &Transform::getValue() {
  return transf;
}

void Transform::accept(NodeVisitor & visitor) {
  visitor.pushOntoNodePath(*this);
  visitor.apply(*this);
  visitor.popFromNodePath();
}

void PGUPV::Transform::render() {
  if (!visible || !getBB().isValid()) return;
  auto bo = PGUPV::gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX);
  auto mats = std::static_pointer_cast<GLMatrices>(bo);

  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->multMatrix(GLMatrices::MODEL_MATRIX, transf.getValue());

  Group::render();

  mats->popMatrix(GLMatrices::MODEL_MATRIX);
}

void Transform::recomputeBoundingBox() {
  Group::recomputeBoundingBox();
  bb.transform(transf.getValue());
}

void Transform::recomputeBoundingSphere() {
  Group::recomputeBoundingSphere();
  bs.transform(transf.getValue());
}

std::shared_ptr<Transform> Transform::shared_from_this()
{
  return std::static_pointer_cast<Transform>(Node::shared_from_this());
}

Transform::Transform(const glm::mat4 &xform) : transf(xform) {
  transf.addListener([this](const glm::mat4&) {invalidateBoundingVolumes(); });
}
