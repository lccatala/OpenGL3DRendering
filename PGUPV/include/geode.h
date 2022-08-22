#pragma once
// 2016
#include "memory.h"
#include "node.h"

namespace PGUPV {
  class Geode : public Node {
  public:
    static std::shared_ptr<Geode> build(std::shared_ptr<Model> m = nullptr);
    void render() override;
    void addMesh(std::shared_ptr<Mesh> m);
    void accept(NodeVisitor &visitor) override;
    Model &getModel() { return *model; }
    void setModel(std::shared_ptr<Model> m);
    std::shared_ptr<Geode> shared_from_this();
  protected:
    Geode();
    Geode(std::shared_ptr<Model> m) : model(m) {};
    std::shared_ptr<Model> model;
    void recomputeBoundingBox() override;
    void recomputeBoundingSphere() override;
  };
};
