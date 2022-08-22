
#include <algorithm>
#include <sstream>
#include <glm/glm.hpp>
#include <assert.h>

#include "node.h"
#include "nodeVisitor.h"
#include "nodeCallback.h"

using PGUPV::Node;
using PGUPV::GLMatrices;
using PGUPV::BoundingBox;
using PGUPV::BoundingSphere;
using PGUPV::Mesh;
using PGUPV::NodeVisitor;
using PGUPV::NodeCallback;
using PGUPV::Group;

uint32_t Node::nextNodeId{ 1 };

BoundingBox Node::getBB() {
  if (!bb.isValid())
    recomputeBoundingBox();
  return bb;
}

BoundingSphere Node::getBS() {
  if (!bs.isValid())
    recomputeBoundingSphere();
  return bs;
}

void Node::resetBB() {
  bb.reset();
}

void Node::resetBS() {
  bs.reset();
}

Node *Node::getParent(unsigned int i) const
{
  return parents[i];
}

void Node::ascend(NodeVisitor & visitor) {
  for (auto n : parents) {
    n->accept(visitor);
  }
}

void Node::remove() {
  for (auto n : parents) {
    n->removeChild(shared_from_this());
  }
  parents.clear();
}

class InvalidateBoundingBoxes : public NodeVisitor {
public:
  InvalidateBoundingBoxes() {
    setTraversalMode(NodeVisitor::TraversalMode::TRAVERSE_PARENTS);
  }
  void apply(Node &node) override {
    node.resetBB();
    node.resetBS();
    node.ascend(*this);
  }
};

void Node::invalidateBoundingVolumes() {
  InvalidateBoundingBoxes ibb;
  accept(ibb);
}

void Node::addUpdateCallback(std::shared_ptr<NodeCallback> nc)
{
	auto it = std::find(updateCallbacks.begin(), updateCallbacks.end(), nc);
	if (it != updateCallbacks.end())
		return;
	updateCallbacks.push_back(nc);
}

void Node::removeUpdateCallback(std::shared_ptr<NodeCallback> nc)
{
	auto it = std::find(updateCallbacks.begin(), updateCallbacks.end(), nc);
	if (it != updateCallbacks.end())
		updateCallbacks.erase(it);
}

void Node::clearUpdateCallbacks()
{
	updateCallbacks.clear();
}

void Node::addParent(Group *parent) {
  assert(parent != this);
  parents.push_back(parent);
}


void Node::removeParent(Group *parent) {
  parents.erase(std::remove(parents.begin(), parents.end(), parent), parents.end());
}
