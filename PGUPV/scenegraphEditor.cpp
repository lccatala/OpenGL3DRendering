#include <sstream>

#include "sceneGraphEditor.h"

#include "scene.h"
#include "nodeVisitor.h"
#include "button.h"                           // for Button
#include "checkBoxWidget.h"                   // for CheckBoxWidget
#include "transformationWidget.h"             // for TransformationWidget
#include "hbox.h"
#include "animatorController.h"
#include "floatSliderWidget.h"
#include "listBoxWidget.h"

#include <IconsFontAwesome5.h>

using PGUPV::SceneGraphEditor;
using PGUPV::Panel;
using PGUPV::Scene;
using PGUPV::NodeVisitor;
using PGUPV::Node;
using PGUPV::Group;
using PGUPV::Transform;
using PGUPV::Geode;
using PGUPV::TransformationWidget;
using PGUPV::Button;
using PGUPV::TreeWidget;
using PGUPV::NodePath;
using PGUPV::HBox;
using PGUPV::TreeNodeFlag;

#define DUMMY_NODE_PREFIX "$%$%"

SceneGraphEditor::SceneGraphEditor(std::shared_ptr<Scene> scene) :
	theScene(scene) {
	build();
}

SceneGraphEditor::~SceneGraphEditor()
{
}

void SceneGraphEditor::refresh() {
	updateListBox();
}

std::string processName(const std::string& name) {
	if (name.length() == 0)
		return "<unknown>";
	return name;
}

//class NodeIds : public NodeVisitor {
//public:
//  NodeIds() : level(0) {};
//
//  void apply(Geode &geode) override {
//    auto pn = processName(geode.getName());
//    if (!PGUPV::starts_with(pn, DUMMY_NODE_PREFIX)) {
//      ids.push_back(std::string(level, '.') + "Geode: " + pn);
//      nodepaths.push_back(getNodePath());
//    }
//    traverse(geode);
//  };
//  void apply(Group &group) override {
//    processGroup("Group: ", group);
//  };
//  void apply(Transform &transform) override {
//    processGroup("Transform: ", transform);
//  }
//  std::vector<std::string> getIds() { return ids; }
//  std::vector<PGUPV::NodePath> getNodePaths() { return nodepaths; }
//private:
//  std::vector<std::string> ids;
//  std::vector<PGUPV::NodePath> nodepaths;
//
//  unsigned long level;
//  void processGroup(std::string type, Group &group) {
//    auto pn = processName(group.getName());
//    if (!PGUPV::starts_with(pn, DUMMY_NODE_PREFIX)) {
//      ids.push_back(std::string(level, '.') + type + pn);
//      nodepaths.push_back(getNodePath());
//    }
//    ++level;
//    traverse(group);
//    --level;
//  }
//};

class NodeIds : public NodeVisitor {
public:
	void apply(Geode& geode) override {
		auto nt = std::make_shared<TreeWidget<NodePath>::TreeNode>("Geode: " + processName(geode.getName()));
		nt->userElement = getNodePath();
		current->addChild(nt);
	};
	void apply(Group& group) override {
		auto g = processGroup("Group: ", group);
		if (g)
			root = g;
	};
	void apply(Transform& transform) override {
		auto t = processGroup("Transform: ", transform);
		if (t)
			root = t;
	}
	void apply(PGUPV::AnimationNode& animation) override {
		auto t = processAnim("AnimNode: ", animation);
		if (t)
			root = t;
	}
	std::shared_ptr<TreeWidget<NodePath>::TreeNode> getRoot() {
		return root;
	}
private:
	std::shared_ptr<TreeWidget<NodePath>::TreeNode> root;
	std::shared_ptr<TreeWidget<NodePath>::TreeNode> current;
	std::shared_ptr<TreeWidget<NodePath>::TreeNode> processGroup(std::string type, Group& group) {
		auto prev = current;
		auto pn = processName(group.getName());
		std::shared_ptr<TreeWidget<NodePath>::TreeNode> nt;
		if (!PGUPV::starts_with(pn, DUMMY_NODE_PREFIX)) {
			nt = std::make_shared<TreeWidget<NodePath>::TreeNode>(processName(type + group.getName()));
			nt->userElement = getNodePath();
			if (current) {
				current->addChild(nt);
			}
			current = nt;
		}
		traverse(group);
		current = prev;
		return nt;
	}

	std::shared_ptr<TreeWidget<NodePath>::TreeNode> processAnim(std::string type, PGUPV::AnimationNode& node) {
		auto prev = current;
		auto pn = processName(node.getName());
		std::shared_ptr<TreeWidget<NodePath>::TreeNode> nt;
		if (!PGUPV::starts_with(pn, DUMMY_NODE_PREFIX)) {
			nt = std::make_shared<TreeWidget<NodePath>::TreeNode>(processName(type + node.getName()));
			nt->userElement = getNodePath();
			if (current) {
				current->addChild(nt);
			}
			current = nt;
		}
		traverse(node);
		current = prev;
		return nt;
	}
};


bool isEditable(std::shared_ptr<Node> node) {
	return node->getNumParents() == 1 && PGUPV::starts_with(node->getParent(0)->getName(), DUMMY_NODE_PREFIX);
}

void SceneGraphEditor::renderWidget() {
	visible->render();
	//removeNodeBtn->render();
	//editNodeBtn->render();
	//resetBtn->render();
	xformsButtons->render();

	sep.render();

	bsLabel.render();
	bsCenterInfo.render();
	bsRadiusInfo.render();

	bbLabel.render();
	bb1Info.render();
	bb2Info.render();

	sep.render();

	transformWidget->render();

	if (!selectedNodePath.empty()) {
		auto animNode = std::dynamic_pointer_cast<AnimationNode>(selectedNodePath.back());
		if (animNode) {
			auto status = animNode->getAnimatorController()->getStatus();
			switch (status) {
			case AnimatorController::Status::Playing:
				playBtn->setLabel(ICON_FA_PAUSE_CIRCLE);
				break;
			case AnimatorController::Status::Stopped:
			case AnimatorController::Status::Paused:
				playBtn->setLabel(ICON_FA_PLAY_CIRCLE);
				break;
			}
			animationButtons->render();
			animationSpeed->render();
			animWrapMode->render();
		}
	}
	sep.render();

	nodeListTree->render();
}

void SceneGraphEditor::setScene(std::shared_ptr<Scene> scene)
{
	theScene = scene;
	refresh();
}

class FindNodeId : public NodeVisitor {
public:
	FindNodeId(uint32_t id) : theId(id) {}
	void apply(Node& node) override {
		if (node.getId() == theId) {
			result = getNodePath();
		}
		else {
			traverse(node);
		}
	}
	NodePath getResult() {
		return result;
	}
private:
	uint32_t theId;
	NodePath result;
};

void SceneGraphEditor::selectNode(uint32_t nodeId)
{
	FindNodeId findId{ nodeId };
	theScene->getRoot()->accept(findId);
	selectNode(findId.getResult());

}

void SceneGraphEditor::build() {
	nodeListTree = std::make_shared<TreeWidget<NodePath>>("Nodes");
	nodeListTree->getValue().addListener([this](const PGUPV::NodePath& np) {
		selectNode(np);
		});
	nodeListTree->setNodeStyle([](const PGUPV::NodePath& np) {
		const auto theNode = np.back();
		PGUPV::TreeNodeFlags flags = static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_None);
		if (std::dynamic_pointer_cast<PGUPV::AnimationNode>(theNode)) {
			flags |= static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_PGUPV_Movie);
		}
		if (theNode->getBB().isValid()) {
			if (theNode->isVisible()) {
				flags |= static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_PGUPV_Visible);
			}
			else {
				flags |= static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_PGUPV_Invisible);
			}
		}
		return flags;
		});

	visible = std::make_shared<CheckBoxWidget>("Visible");
	visible->getValue().addListener([this](const bool& vis) {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			// En un nodepath, el primer elemento es el más cercano a la raíz, y el último el nodo de interés
			np.back()->setVisible(vis);
		}
		});

	removeNodeBtn = std::make_shared<Button>("Remove node", [this]() {
		theScene->removeSelectedNodes();
		updateListBox();
		});

	editNodeBtn = std::make_shared<Button>("Edit node", [this]() {
		NodePath np;
		if (nodeListTree->getSelected(np))
			editNode(np);
		});

	resetBtn = std::make_shared<Button>("Reset transform", [this]() {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			auto n = np.back();
			if (isEditable(n)) {
				transformWidget->setMatrix(glm::mat4(1.0f));
			}
		}
		});

	xformsButtons = std::make_shared<HBox>();
	xformsButtons->addChild(removeNodeBtn);
	xformsButtons->addChild(editNodeBtn);
	xformsButtons->addChild(resetBtn);

	playBtn = std::make_shared<Button>(ICON_FA_PLAY_CIRCLE, [this]() {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			auto n = np.back();
			auto animNode = std::dynamic_pointer_cast<AnimationNode>(n);
			if (animNode) {
				auto controller = animNode->getAnimatorController();
				switch (controller->getStatus())
				{
				case AnimatorController::Status::Stopped:
					controller->start();
					break;
				case AnimatorController::Status::Playing:
					controller->pause();
					break;
				case AnimatorController::Status::Paused:
					controller->start();
					break;
				}
			}
		}
		});

	stopBtn = std::make_shared<Button>(ICON_FA_STOP_CIRCLE, [this]() {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			auto n = np.back();
			auto animNode = std::dynamic_pointer_cast<AnimationNode>(n);
			if (animNode) {
				auto controller = animNode->getAnimatorController();
				controller->stop();
			}
		}
		});

	animationButtons = std::make_shared<HBox>();
	animationButtons->addChild(playBtn);
	animationButtons->addChild(stopBtn);

	animationSpeed = std::make_shared<FloatSliderWidget>("Anim. speed", 1.0f, 0.1f, 4.0f);
	animationSpeed->getValue().addListener([this](const float& v) {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			auto animNode = std::dynamic_pointer_cast<AnimationNode>(np.back());
			if (animNode) {
				animNode->getAnimatorController()->currentState()->setSpeed(v);
			}
		}
		});

	animWrapMode = std::make_shared<ListBoxWidget<AnimationClip::WrapMode>>("Anim. mode",
		std::vector<std::string> {"ONCE", "LOOP", "PING_PONG", "CLAMP_FOREVER"}, 0);
	animWrapMode->setUserData(std::vector< AnimationClip::WrapMode> {
		AnimationClip::WrapMode::ONCE,
			AnimationClip::WrapMode::LOOP,
			AnimationClip::WrapMode::PING_PONG,
			AnimationClip::WrapMode::CLAMP_FOREVER
	});
	animWrapMode->getValue().addListener([this](const int&) {
		NodePath np;
		if (nodeListTree->getSelected(np)) {
			auto animNode = std::dynamic_pointer_cast<AnimationNode>(np.back());
			if (animNode) {
				animNode->getAnimatorController()->currentState()->getAnimationClip()->setWrapMode(
					animWrapMode->getSelectedUserData());
			}
		}
		});

	bsLabel.setText("Bounding Sphere");
	bbLabel.setText("Bounding Box");

	transformWidget = std::make_shared<TransformationWidget>("Transformation", glm::mat4(1.0f));
	refresh();
}



void SceneGraphEditor::editNode(const NodePath& np) {
	if (!theScene->getRoot())  // Empty scene
		return;
	auto n = np.back();
	INFO("Editando nodo " + n->getName());
	Transform* dummyNode;

	if (isEditable(n)) {
		dummyNode = static_cast<Transform*>(n->getParent(0));
	}
	else {
		// Create a dummy transform node and set it as a parent for this node
		auto t = Transform::build();
		t->setName(DUMMY_NODE_PREFIX + n->getName());
		auto sharedn = n->shared_from_this();
		if (n->getNumParents() == 0) {
			theScene->setRoot(t);
		}
		else {
			while (n->getNumParents() > 0) {
				auto p = static_cast<Group*>(n->getParent(0));
				p->removeChild(n);
				p->addChild(t);
			}
		}
		t->addChild(sharedn);
		dummyNode = t.get();
	}

	// the only parent of n is a dummy node. Connect it with the edit widgets
	float size;
	if (n->getBB().isValid())
		size = n->getBB().getMaxDimension();
	else
		size = 5.0f;
	transformWidget->setTranslationRange(-2.0f * size, 2.0f * size);
	transformWidget->getValue().setValue(dummyNode->getTransform());
	dummyNode->getValue().bind(transformWidget->getValue());
	transformWidget->setVisible(true);
}


bool nodePathExists(const NodePath nodePath, size_t idx, std::shared_ptr<Node> root);

bool nodePathExists(const NodePath nodePath, std::shared_ptr<Node> root) {
	return nodePathExists(nodePath, 0, root);
}

bool nodePathExists(const NodePath nodePath, size_t idx, std::shared_ptr<Node> root) {
	if (idx >= nodePath.size())
		return true;
	if (!root)
		return false;

	if (PGUPV::starts_with(root->getName(), DUMMY_NODE_PREFIX)) {
		std::shared_ptr<Group> g = std::dynamic_pointer_cast<Group>(root);
		if (!g)
			return false;
		assert(g->getNumChildren() == 1);
		return nodePathExists(nodePath, idx, g->getChild(0));
	}

	if (nodePath[idx].get() != root.get()) {
		return false;
	}

	if (idx == nodePath.size() - 1)
		return true;
	else {
		std::shared_ptr<Group> g = std::dynamic_pointer_cast<Group>(root);
		if (!g || !g->containsChild(nodePath[idx + 1]))
			return false;
		return nodePathExists(nodePath, idx + 1, nodePath[idx + 1]);
	}
}

//class NodePathMatches : public NodeVisitor {
//public:
//  NodePathMatches(const NodePath &np) :
//    np(np), root(root), checking(0), result(true)
//  {
//  }
//  void apply(Node &n) override {
//    if (checking >= np.size())
//      return; // Matches
//    if (&n != np[checking].get()) {
//      result = false; // Does not match
//    }
//  }

//  void apply(Group &n) override {
//    if (&n != np[checking].get()) {
//      result = false;
//    }
//    else {
//      if (++checking < np.size()) {
//        // Search the child
//        for (size_t i = 0; i < n.getNumChildren(); i++) {
//          auto node = n.getChild(i).get();
//          if (node == np[checking].get()) {
//            apply(*node);
//            return;
//          }
//        }
//        result = false;
//      }
//    }
//  }
//  bool getResult() {
//    return result;
//  }
//private:
//  const NodePath &np;
//  std::shared_ptr<Node> root;
//  size_t checking;
//  bool result;
//};

//NodePathMatches npe(np);
//root->accept(npe);
//return npe.getResult();

void SceneGraphEditor::selectNode(const NodePath& np) {

	// If there was a node in edition, disconect it from the GUI
	if (!selectedNodePath.empty() && isEditable(selectedNodePath.back())) {
		auto t = static_cast<Transform*>(selectedNodePath.back()->getParent(0));
		t->getValue().unbind();
	}

	if (!theScene->getRoot()) {// Empty scene
		selectedNodePath.clear();
		return;
	}

	if (!nodePathExists(np, theScene->getRoot()))
		ERRT("Ese nodo no existe");

	theScene->unselectAll();
	selectedNodePath = np;
	auto lastNode = selectedNodePath.back();
	lastNode->select(true);
	visible->set(lastNode->isVisible());

	if (lastNode->getBS().isValid()) {
		bsCenterInfo.setText("Center: " + to_string(lastNode->getBS().center));
		bsRadiusInfo.setText("Radius: " + std::to_string(lastNode->getBS().radius));

		bb1Info.setText("Corner min:" + to_string(lastNode->getBB().min));
		bb2Info.setText("Corner max:" + to_string(lastNode->getBB().max));
	}
	else {
		bsCenterInfo.setText("Empty");
		bsRadiusInfo.setText("");

		bb1Info.setText("Empty");
		bb2Info.setText("");
	}

	if (isEditable(lastNode)) {
		editNode(np);
	}
	else {
		if (transformWidget)
			transformWidget->setVisible(false);
	}

	auto animNode = std::dynamic_pointer_cast<AnimationNode>(np.back());
	if (animNode) {
		animationSpeed->set(animNode->getAnimatorController()->currentState()->getSpeed(), false);
		auto wrap = animNode->getAnimatorController()->currentState()->getAnimationClip()->getWrapMode();
		animWrapMode->setSelected(wrap, false);
	}
}

void SceneGraphEditor::updateListBox() {
	NodeIds nids;
	if (theScene->getRoot()) {
		theScene->getRoot()->accept(nids);
		auto dim = theScene->getBB().getMaxDimension();
		transformWidget->setTranslationRange(-dim * 2.0f, dim * 2.0f);
	}
	auto root = nids.getRoot();
	nodeListTree->setElements(root);
	if (root) {
		nodeListTree->select(root);
		selectNode(root->userElement);
	}
}
