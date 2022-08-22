#pragma once

#include "nodeVisitor.h"
#include "nodeCallback.h"

namespace PGUPV {
	class UpdateVisitor : public NodeVisitor {
	public:
		UpdateVisitor() : NodeVisitor(NodeVisitor::TraversalMode::TRAVERSE_ALL_CHILDREN) {};

		/** During traversal each type of node calls its callbacks and its children traversed. */
		virtual void apply(Node& node) override { handle_callbacks_and_traverse(node); }
		virtual void apply(Geode& node) override { handle_geode_callbacks(node); }

	protected:
		inline void handle_callbacks_and_traverse(Node& node)
		{
			for (auto callback : node.getUpdateCallbacks()) {
				(*callback)(node, *this);
			}
			traverse(node);
		}

		inline void handle_geode_callbacks(Geode& geode)
		{
			for (auto callback : geode.getUpdateCallbacks()) {
				(*callback)(geode, *this);
			}
		}
	};
};