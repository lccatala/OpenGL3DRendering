#pragma once
// 2016
#include "node.h"
#include "group.h"
#include "transform.h"
#include "geode.h"
#include "animationNode.h"

namespace PGUPV {
	class NodeVisitor {
	public:
		enum class TraversalMode {
			TRAVERSE_NONE, TRAVERSE_PARENTS, TRAVERSE_ALL_CHILDREN, TRAVERSE_ACTIVE_CHILDREN
		};
		NodeVisitor(TraversalMode mode = TraversalMode::TRAVERSE_ACTIVE_CHILDREN) :
			traversalMode(mode) {};
		virtual void apply(Node &node) {
			traverse(node);
		};
		virtual void apply(AnimationNode &node) {
			apply(static_cast<Node &>(node));
		}
		virtual void apply(Group &group) {
			apply(static_cast<Node &>(group));
		}
		virtual void apply(Transform &transform) {
			apply(static_cast<Group &>(transform));
		}
		virtual void apply(Geode &geode) {
			apply(static_cast<Node &>(geode));
		}
		inline void setTraversalMode(TraversalMode mode) { traversalMode = mode; }
		/** Get the traversal mode.*/
		inline TraversalMode getTraversalMode() const { return traversalMode; }


		//// Este código viene directamente de OSG::NodeVisitor
		/** Method for handling traversal of a nodes.
		If you intend to use the visitor for actively traversing
		the scene graph then make sure the accept() methods call
		this method unless they handle traversal directly.*/
		inline void traverse(Node &node)
		{
			if (traversalMode == TraversalMode::TRAVERSE_PARENTS) {
				node.ascend(*this);
			}
			else if (traversalMode != TraversalMode::TRAVERSE_NONE) {
				node.traverse(*this);
			}
		}


		/** Method called by osg::Node::accept() method before
		* a call to the NodeVisitor::apply(..).  The back of the list will,
		* therefore, be the current node being visited inside the apply(..),
		* and the rest of the list will be the parental sequence of nodes
		* from the top most node applied down the graph to the current node.
		* Note, the user does not typically call pushNodeOnPath() as it
		* will be called automatically by the Node::accept() method.*/
		inline void pushOntoNodePath(Node &node) {
			if (traversalMode != TraversalMode::TRAVERSE_PARENTS)
				nodepath.push_back(node.shared_from_this());
			else
				nodepath.insert(nodepath.begin(), node.shared_from_this());
		}

		/** Method called by osg::Node::accept() method after
		* a call to NodeVisitor::apply(..).
		* Note, the user does not typically call popFromNodePath() as it
		* will be called automatically by the Node::accept() method.*/
		inline void popFromNodePath() {
			if (traversalMode != TraversalMode::TRAVERSE_PARENTS)
				nodepath.pop_back();
			else
				nodepath.erase(nodepath.begin());
		}


		const NodePath &getNodePath() const {
			return nodepath;
		}

	protected:
		TraversalMode traversalMode;
		NodePath nodepath;
	};
};
