#include <algorithm>

#include "group.h"
#include "nodeVisitor.h"
#include "node.h"

using PGUPV::Group;
using PGUPV::NodeVisitor;
using PGUPV::Node;

std::shared_ptr<Group> Group::build()
{
  return std::shared_ptr<Group>(new Group());
}

void Group::render() {
  if (!visible || !getBB().isValid()) return;
  for (auto &c : children)
    c->render();
}

void Group::recomputeBoundingBox() {
  // Compute the BB for my children
  for (const auto &c : children) {
    bb.grow(c->getBB());
  }
}

void Group::recomputeBoundingSphere() {
  // Compute the BB for my children
  for (const auto &c : children) {
    bs.grow(c->getBS());
  }
}

void Group::traverse(NodeVisitor &visitor) {
  for (auto &c : children) {
    c->accept(visitor);
  }
}

void Group::accept(NodeVisitor & visitor) {
  visitor.pushOntoNodePath(*this);
  visitor.apply(*this);
  visitor.popFromNodePath();
}

std::shared_ptr<Group> Group::shared_from_this()
{
  return std::static_pointer_cast<Group>(Node::shared_from_this());
}

void Group::addChild(std::shared_ptr<Node> n) {
  invalidateBoundingVolumes();
  children.push_back(n);
  n->addParent(this);
}
void Group::removeChild(std::shared_ptr<Node> n) {
  n->removeParent(this);
  children.erase(std::remove(children.begin(), children.end(), n), children.end());
  invalidateBoundingVolumes();
}

void Group::removeChild(Node *n) {
  for (size_t i = 0; i < children.size(); i++) {
    if (children[i].get() == n) {
      removeChild(i);
      return;
    }
  }
}

void Group::removeChild(size_t i) {
  // Nos quitamos como padre...
  children[i]->removeParent(this);
  // Mantenemos el orden de los hijos...
  children.erase(children.begin() + i);
  invalidateBoundingVolumes();
}

void Group::removeChildren()
{
	while (getNumChildren() > 0) {
		removeChild(getNumChildren() - 1);
	}
}

bool Group::containsChild(std::shared_ptr<Node> n)
{
	for (const auto &child : children) {
		if (child.get() == n.get())
			return true;
	}
	return false;
}

