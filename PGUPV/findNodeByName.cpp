#include "findNodeByName.h"
#include "utils.h"

using PGUPV::FindNodeByName;
using PGUPV::Node;

FindNodeByName::FindNodeByName(const std::string &str) : str(str) {
}

void FindNodeByName::apply(Node &node) {
  if (PGUPV::containsIgnoreCase(node.getName(), str))
    result.push_back(node.shared_from_this());
  traverse(node);
}

const std::vector<std::shared_ptr<Node>> FindNodeByName::getResults() {
  return result;
}
