#include "renderable.h"

using PGUPV::Renderable;
using PGUPV::BoundingBox;

Renderable::~Renderable() {
}

glm::vec3 Renderable::center() {
  glm::vec3 c;
  BoundingBox bb = getBB();
  c = bb.min + (bb.max - bb.min) / 2.0f;
  return c;
}

float Renderable::maxDimension() {
  BoundingBox bb = getBB();
  float tmp = bb.max.x - bb.min.x;
  tmp = MAX(tmp, bb.max.y - bb.min.y);
  tmp = MAX(tmp, bb.max.z - bb.min.z);
  return tmp;
}