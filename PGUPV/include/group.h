#pragma once
// 2016

#include <memory>
#include "node.h"

namespace PGUPV {
  class Group : public Node {
  public:
    static std::shared_ptr<Group> build();
    void render() override;
    virtual void addChild(std::shared_ptr<Node> n);
    size_t getNumChildren() { return children.size(); };
    void removeChild(std::shared_ptr<Node> n);
    void removeChild(Node *n);
    void removeChild(size_t i);
	void removeChildren();
	bool containsChild(std::shared_ptr<Node> n);
    std::shared_ptr<Node> getChild(size_t i) const { return children[i]; }
    void traverse(NodeVisitor &visitor) override;
    void accept(NodeVisitor &visitor) override;
    std::shared_ptr<Group> shared_from_this();
  protected:
    Group() {};
    std::vector<std::shared_ptr<Node>> children;
    void recomputeBoundingBox() override;
    void recomputeBoundingSphere() override;
  };
};
