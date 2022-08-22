#pragma once

#include "group.h"
#include <glm/mat4x4.hpp>

namespace PGUPV {
	class AnimatorController;
	class NodeVisitor;
	class Mesh;

	class AnimationNode : public Node {
	public:
		AnimationNode(std::shared_ptr<Node> root);
		void setAnimatorController(std::shared_ptr<AnimatorController> animatorController);
		std::shared_ptr<AnimatorController> getAnimatorController() {
			return animController;
		}
		void render() override;
		void accept(NodeVisitor &dispatcher) override;
		void ascend(NodeVisitor &visitor) override;
		void traverse(NodeVisitor &) override;
	protected:
		void recomputeBoundingBox() override;
		void recomputeBoundingSphere() override;
	private:
		void build();
		std::shared_ptr<Node> subScene;
		std::shared_ptr<AnimatorController> animController;
		std::map<Mesh *, glm::mat4> worldMatrix, inverseWorldMatrix;
		std::vector<Mesh *> meshes;
	};
};
