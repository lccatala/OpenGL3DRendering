#pragma once

#include "node.h"
#include "nodeVisitor.h"

namespace PGUPV {
	class NodeCallback {
	public:
		virtual void operator()(Node &node, NodeVisitor &nv) {
			traverse(node, nv);
		}

		/** Call any nested callbacks and then traverse the scene graph. */
		void traverse(Node& node, NodeVisitor& nv) {
			nv.traverse(node);
		}
	};
};