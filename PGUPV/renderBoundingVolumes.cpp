#include "common.h"
#include "indexedBindingPoint.h"
#include "glMatrices.h"
#include "group.h"
#include "transform.h"
#include "geode.h"
#include "renderBoundingVolumes.h"
#include "stockModels.h"
#include "colorWheel.h"
#include "boundingVolumes.h"
#include "stockPrograms.h"
#include "hacks.h"
#include "glStateCache.h"

using PGUPV::RenderBoundingVolumes;
using PGUPV::Group;
using PGUPV::Transform;
using PGUPV::Geode;
using PGUPV::GLMatrices;
using PGUPV::BoundingBox;
using PGUPV::BoundingSphere;
using PGUPV::WireBox;
using PGUPV::Sphere;
using PGUPV::PolygonModeState;

std::unique_ptr<WireBox> RenderBoundingVolumes::box;
std::unique_ptr<Sphere> RenderBoundingVolumes::sphere;


#define SELECTED_NODE_WIDTH_BBOX 4.0f
#define GEODE_COLOR glm::vec4(.7f, .7f, .7f, 1.0f)
#define GROUP_COLOR glm::vec4(.2f, .7f, .2f, 1.0f)
#define TRANSFORM_COLOR glm::vec4(.7f, .2f, .2f, 1.0f)

void RenderBoundingVolumes::apply(Group &group) {
  if (group.isVisible()) {
    if (volume == Volume::BOX)
      renderBBox(group.getBB(), group.isSelected(), GROUP_COLOR);
    else
      renderBSph(group.getBS(), group.isSelected(), GROUP_COLOR);
    traverse(group);
  }
}

void RenderBoundingVolumes::apply(Transform &transform) {
  if (transform.isVisible()) {
    if (volume == Volume::BOX)
      renderBBox(transform.getBB(), transform.isSelected(), TRANSFORM_COLOR);
    else
      renderBSph(transform.getBS(), transform.isSelected(), TRANSFORM_COLOR);
    auto prev = current;
    current = current * transform.getTransform();
    traverse(transform);
    current = prev;
  }
}

void RenderBoundingVolumes::apply(Geode &geode) {
  if (geode.isVisible()) {
    if (volume == Volume::BOX) {
      renderBBox(geode.getBB(), geode.isSelected(), GEODE_COLOR);
      for (size_t i = 0; i < geode.getModel().getNMeshes(); i++)
        renderBBox(geode.getModel().getMesh(i).getBB(), geode.isSelected(), GEODE_COLOR);

    }
    else {
      renderBSph(geode.getBS(), geode.isSelected(), GEODE_COLOR);
      for (size_t i = 0; i < geode.getModel().getNMeshes(); i++)
        renderBSph(geode.getModel().getMesh(i).getBS(), geode.isSelected(), GEODE_COLOR);
    }
  }
}

void RenderBoundingVolumes::reset() {
  lastColor = 0;
}

void RenderBoundingVolumes::renderBBox(const BoundingBox &bb, bool highlight, const glm::vec4 &color) {
  if (!bb.isValid())
    return;
  auto bo = PGUPV::gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX);
  auto mats = std::static_pointer_cast<GLMatrices>(bo);
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->setMatrix(GLMatrices::MODEL_MATRIX, current);
  auto prevprog = PGUPV::ConstantIllumProgram::use();

  if (!box) box = std::unique_ptr<WireBox>(new WireBox());
  Mesh &m = box->getMesh(0);
  m.setColor(color);
  auto vbo = m.getBufferObject(Mesh::VERTICES);
  gl_copy_write_buffer.bind(vbo);
  auto vertices = bb.getVertices();
  gl_copy_write_buffer.write(&vertices[0]);
  if (highlight) {
    glLineWidth(SELECTED_NODE_WIDTH_BBOX);
  }
  box->render();
  if (highlight) {
    glLineWidth(1.0f);
  }
  prevprog->use();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);
}

void RenderBoundingVolumes::renderBSph(const BoundingSphere &bs, bool highlight, const glm::vec4 &color) {
  if (!bs.isValid())
    return;
  auto bo = PGUPV::gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX);
  auto mats = std::static_pointer_cast<GLMatrices>(bo);
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->setMatrix(GLMatrices::MODEL_MATRIX, current);
  mats->translate(GLMatrices::MODEL_MATRIX, bs.center);
  mats->scale(GLMatrices::MODEL_MATRIX, bs.radius * 2.0f);

  auto prevprog = PGUPV::ConstantIllumProgram::use();

  if (!sphere) sphere = std::unique_ptr<Sphere>(new Sphere(0.5f));
  Mesh &m = sphere->getMesh(0);
  m.setColor(color);
  if (highlight) {
    glLineWidth(SELECTED_NODE_WIDTH_BBOX);
  }
  GLStateCapturer<PolygonModeState> pm;
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  sphere->render();

  pm.restore();
  if (highlight) {
    glLineWidth(1.0f);
  }
  prevprog->use();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);
}
