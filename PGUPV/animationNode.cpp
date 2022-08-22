#include "animationNode.h"
#include "animatorController.h"
#include "nodeVisitor.h"
#include "transform.h"
#include "geode.h"
#include "matrixStack.h"
#include "uboBones.h"
#include "indexedBindingPoint.h"
#include "glMatrices.h"
#include "skeleton.h"
#include "bone.h"
#include "nodeCallback.h"
#include "app.h"

#include <glm/gtc/matrix_inverse.hpp>

using PGUPV::AnimationNode;
using PGUPV::AnimatorController;
using PGUPV::AnimatorState;
using PGUPV::Node;
using PGUPV::NodeVisitor;
using PGUPV::Mesh;
using PGUPV::Skeleton;
using PGUPV::Bone;

class Updater : public PGUPV::NodeCallback {
public:
	Updater() {};
	void operator()(Node &node, NodeVisitor &nv) override;
};

void Updater::operator()(Node & node, NodeVisitor & nv)
{
	auto &an = static_cast<AnimationNode &>(node);
	an.getAnimatorController()->update(PGUPV::App::getDeltaTime());
	traverse(node, nv);
}


AnimationNode::AnimationNode(std::shared_ptr<Node> root) : subScene(root)
{
	build();
	
	addUpdateCallback(std::make_shared<Updater>());
}



class AnimatedMeshRenderer : public NodeVisitor {


};


class BoneMatricesUpdater : public PGUPV::NodeVisitor {
public:
	BoneMatricesUpdater(
		const glm::mat4 &currentWCS,
		std::shared_ptr<AnimatorState> anim,
		std::vector<glm::mat4> &boneMatrices,
		PGUPV::Skeleton &skeleton
	) : animation(anim), boneMats(boneMatrices), skel(skeleton) {
		matstack.setMatrix(currentWCS);
	};

	void apply(PGUPV::Transform &transform) override {
		matstack.pushMatrix();
		glm::mat4 mat;
		if (animation && animation->interpolate(transform.getName(), mat))
			matstack.multMatrix(mat);
		else
			matstack.multMatrix(transform.getTransform());

		auto b = skel.getBoneIndex(transform.getName());
		if (b != PGUPV::Skeleton::NOBONE) {
			boneMats[b] = matstack.getMatrix() * skel.getBone(b)->getMatrix();

		}
		traverse(transform);

		matstack.popMatrix();
	}
private:
	PGUPV::MatrixStack matstack;
	std::shared_ptr<AnimatorState> animation;
	std::vector<glm::mat4> &boneMats;
	PGUPV::Skeleton &skel;
};

void AnimationNode::render()
{
	std::vector<glm::mat4> boneMatrices(UBOBones::MAX_BONES, glm::mat4(1.0f));

	auto mats = std::dynamic_pointer_cast<GLMatrices>(gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX));

	for (auto m : meshes) {
		std::shared_ptr<Skeleton> skeleton = m->getSkeleton();
		if (!skeleton)
			continue;

		BoneMatricesUpdater updater(
			//mats->getMatrix(GLMatrices::MODEL_MATRIX), 
			glm::mat4(1.0f),
			animController->currentState(), 
			boneMatrices, 
			*skeleton);
		subScene->accept(updater);

		auto nBones = skeleton->getNBones();
		std::shared_ptr<UBOBones> ubobones = m->getBones();
		if (nBones == 0 || !ubobones) {
			continue;
		}
		gl_uniform_buffer.bindBufferBase(ubobones, UBO_BONES_BINDING_INDEX);
		gl_uniform_buffer.write((void *)&boneMatrices[0], nBones * sizeof(glm::mat4), 0);

		mats->pushMatrix(GLMatrices::MODEL_MATRIX);
		auto itWCS = worldMatrix.find(m);
		if (itWCS != worldMatrix.end()) {
			mats->multMatrix(GLMatrices::MODEL_MATRIX, itWCS->second);
		}

		auto it = inverseWorldMatrix.find(m);
		if (it != inverseWorldMatrix.end()) {
			mats->multMatrix(GLMatrices::MODEL_MATRIX, it->second);
		}
		m->render();
		mats->popMatrix(GLMatrices::MODEL_MATRIX);
	};
}

void AnimationNode::accept(NodeVisitor & dispatcher)
{
	dispatcher.pushOntoNodePath(*this);
	dispatcher.apply(*this);
	dispatcher.popFromNodePath();
}

void AnimationNode::ascend(NodeVisitor & visitor)
{
	subScene->traverse(visitor);
}

void AnimationNode::traverse(NodeVisitor &visitor)
{
	subScene->traverse(visitor);
}

void AnimationNode::recomputeBoundingBox()
{
	subScene->recomputeBoundingBox();
	bb = subScene->getBB();
}

void AnimationNode::recomputeBoundingSphere()
{
	subScene->recomputeBoundingSphere();
	bs = subScene->getBS();
}

class ComputeWorldMatrices : public NodeVisitor {
public:
	ComputeWorldMatrices(
		std::vector<Mesh *> &meshes,
		std::map<Mesh *, glm::mat4> &worldMatrixMap,
		std::map<Mesh *, glm::mat4> &inverseWorldMatrixMap) :
		allMeshes(meshes),
		WCS(worldMatrixMap),
		inverseWCS(inverseWorldMatrixMap) {
		allMeshes.clear();
		WCS.clear();
		inverseWCS.clear();
	};
	void apply(PGUPV::Transform &transform) override {
		if (!transform.getBB().isValid())
			return;
		stack.pushMatrix();
		stack.multMatrix(transform.getTransform());
		traverse(transform);
		stack.popMatrix();
	}

	void apply(PGUPV::Geode &geode) override {
		geode.getModel().accept([=](Mesh &m) {
			allMeshes.push_back(&m);
			WCS[&m] = stack.getMatrix();
			inverseWCS[&m] = glm::inverse(stack.getMatrix());
		});
	}
private:
	PGUPV::MatrixStack stack;
	std::vector<Mesh *> &allMeshes;
	std::map<Mesh *, glm::mat4> &WCS, &inverseWCS;
};

void AnimationNode::setAnimatorController(std::shared_ptr<AnimatorController> animatorController) {
	animController = animatorController;
}

void AnimationNode::build()
{
	// Calcular inversas
	ComputeWorldMatrices ci(meshes, worldMatrix, inverseWorldMatrix);
	subScene->accept(ci);
}

