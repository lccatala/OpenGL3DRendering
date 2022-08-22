#include <glm/glm.hpp>

#include "scene.h"
#include "describeScenegraph.h"
#include "findNodeByName.h"
#include "animationClip.h"
#include "updateVisitor.h"
#include "baseMaterial.h"

using PGUPV::Node;
using PGUPV::Scene;
using PGUPV::GLMatrices;
using PGUPV::BoundingBox;
using PGUPV::BoundingSphere;
using PGUPV::Mesh;
using PGUPV::AnimationClip;
using PGUPV::BaseMaterial;


Scene::Scene() {
}

void Scene::setRoot(std::shared_ptr<Node> root) {
	sceneRoot = root;
}

void Scene::render() {
	if (sceneRoot)
		sceneRoot->render();
}

BoundingBox Scene::getBB() {
	if (sceneRoot)
		return sceneRoot->getBB();
	else 
		return BoundingBox();
}

BoundingSphere Scene::getBS() {
  if (sceneRoot)
    return sceneRoot->getBS();
  else
    return BoundingSphere();
}

void Scene::release() {
  sceneRoot.reset();
}

void Scene::print(std::ostream & os) {
  if (sceneRoot) {
    PGUPV::DescribeScenegraph d(std::cout);
    sceneRoot->accept(d);
  }
  else {
    os << "La escena está vacía\n";
  }
}

void Scene::update(unsigned int )
{
	PGUPV::UpdateVisitor update;
	if (sceneRoot)
		sceneRoot->accept(update);
}


void Scene::selectNodesByName(const std::string & substr) {
  PGUPV::FindNodeByName fnbn(substr);
  sceneRoot->accept(fnbn);
  for (auto n : fnbn.getResults()) {
    n->select(true);
  }
}

void Scene::unselectAll() {
  class Unselect : public NodeVisitor {
  public:
    void apply(Node &n) override { n.select(false); traverse(n); }
  };
  Unselect un;
  sceneRoot->accept(un);
}

void Scene::removeSelectedNodes() {
  if (sceneRoot->isSelected()) {
    // Borrar toda la escena
    sceneRoot.reset();
    return;
  }
  class RemoveSelected : public NodeVisitor {
  public:
    void apply(Node &n) override { 
      if (n.isSelected())
        n.remove();
    }
    void apply(Group &g) override {
      if (g.isSelected())
        g.remove();
      else {
        // Remove selected children
        size_t i = 0; 
        while (i < g.getNumChildren()) {
          if (g.getChild(i)->isSelected())
            g.removeChild(i);
          else
            i++;
        }
        // Then traverse the remaining
        traverse(g);
      }
    }
  };
  RemoveSelected rs;
  sceneRoot->accept(rs);
}

void Scene::processMeshes(std::function<void(Mesh &)> op) {
  class TraverseMeshes : public NodeVisitor {
  public:
    TraverseMeshes(std::function<void(Mesh &)> op) : op(op) {};
    void apply(Node &n) override { if (n.getBB().isValid()) traverse(n); }
    void apply(Geode &g) override { g.getModel().accept(op); }
  private:
    std::function<void(Mesh &)> op;
  };
  TraverseMeshes tm(op);
  sceneRoot->accept(tm);
}



std::shared_ptr<BaseMaterial> Scene::getMaterial(const std::string & name)
{
	for (auto m : materials) {
		if (m->getName() == name) {
			return m;
		}
	}
	return std::shared_ptr<BaseMaterial>();
}


void Scene::addAnimation(std::shared_ptr<AnimationClip> animation) {
	animations.push_back(animation);

	INFO("New anim: " + animation->getName());
}

size_t Scene::getNumAnimations() const {
	return animations.size();
}

std::shared_ptr<AnimationClip> PGUPV::Scene::getAnimation(size_t index) const
{
	return animations[index];
}
