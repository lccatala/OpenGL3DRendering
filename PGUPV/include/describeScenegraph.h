#pragma once
// 2016

#include <memory>
#include "nodeVisitor.h"

namespace PGUPV {
  /**
  \class DescribeScenegraph
  Ejemplo de clase que implementa el patrón de NodeVisitor para describir
  la jerarquía de un grafo de escena.
  */

  class Group;
  class Transform;
  class Geode;

  class DescribeScenegraph : public NodeVisitor {
  public:
    DescribeScenegraph(std::ostream &os, size_t spacesPerIndent = 2) : 
      os(os), spacesPerIndent(spacesPerIndent), currentLevel(0) {};
    void apply(Group &group) override;
    void apply(Transform &transform) override;
    void apply(Geode &geode) override;
  private:
    std::ostream &os;
    size_t spacesPerIndent;
    size_t currentLevel;
  };
};