#include "geode.h"
#include "nodeVisitor.h"

using PGUPV::Geode;
using PGUPV::NodeVisitor;
using PGUPV::Model;
using PGUPV::Mesh;



Geode::Geode() :
  model(std::make_shared<Model>()) {}


std::shared_ptr<Geode> PGUPV::Geode::build(std::shared_ptr<Model> m)
{
  if (m == nullptr)
    return std::shared_ptr<Geode>(new Geode());
  return std::shared_ptr<Geode>(new Geode(m));
}

void Geode::render() {
  if (visible)
    model->render();
}

void Geode::recomputeBoundingBox() {
  bb = model->getBB();
}

void Geode::recomputeBoundingSphere() {
  bs = model->getBS();
}


void Geode::setModel(std::shared_ptr<Model> m) {
  model = m;
  bb.reset();
}

std::shared_ptr<Geode> Geode::shared_from_this()
{
  return std::static_pointer_cast<Geode>(Node::shared_from_this());
}

void Geode::addMesh(std::shared_ptr<Mesh> m) { 
  invalidateBoundingVolumes();
  model->addMesh(m);
};

void Geode::accept(NodeVisitor & visitor) {
  visitor.pushOntoNodePath(*this);
  visitor.apply(*this);
  visitor.popFromNodePath();
}
