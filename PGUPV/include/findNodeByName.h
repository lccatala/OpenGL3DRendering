#pragma once
// 2016

#include <string>
#include <vector>
#include <memory>
#include "nodeVisitor.h"

namespace PGUPV {
  class Node;

  class FindNodeByName : public NodeVisitor {
  public:
    FindNodeByName(const std::string &str);
    void apply(Node &node) override;
    const std::vector<std::shared_ptr<Node>> getResults();
  private:
    std::string str;
    std::vector<std::shared_ptr<Node>> result;

  };
};
