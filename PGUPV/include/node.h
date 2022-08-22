
#ifndef _NODE_H
#define _NODE_H 2013

#include <memory>
#include "glMatrices.h"
#include "boundingVolumes.h"
#include "mesh.h"
#include "model.h"

namespace PGUPV {

	/* Una escena es un objeto compuesto por diferentes nodos. Cada nodo contien un modelo, que puede
	estar compuesto por varios Meshes, y diferentes modelos pueden compartir un mismo
	Mesh. P.e., un coche está compuesto por cinco modelos (chasis + cuatro ruedas),
	donde cada rueda apunta al mismo Mesh. Cada modelo llevará asociado una transformación.
	*/

	class Model;
	class NodeVisitor;
	class Node;
	class Group;
	class NodeCallback;

	typedef std::vector<std::shared_ptr<Node>> NodePath;

	class Node : public std::enable_shared_from_this<Node> {
	public:
		Node() : selected(false), visible(true), nodeId{ nextNodeId++ }{};
		virtual ~Node() = default;
		virtual void render() = 0;
		BoundingBox getBB();
		void resetBB();
		BoundingSphere getBS();
		void resetBS();
		Node* getParent(unsigned int i) const;
		size_t getNumParents() const { return parents.size(); };
		std::vector<Node*> getParents();
		void setName(const std::string& nodeName) { name = nodeName; };
		std::string getName() { return name; };
		virtual void accept(NodeVisitor& dispatcher) = 0;
		/**
		Recorre el grafo hacia la raíz
		\param visitor el NodeVisitor que se usará para recorrer el grafo
		*/
		virtual void ascend(NodeVisitor& visitor);
		/**
		Recorre los hijos del nodo
		\param visitor el NodeVisitor que se usará para recorrer el grafo
		*/
		virtual void traverse(NodeVisitor&) {};

		/**
		Saca el nodo fuera del grafo de escena
		*/
		void remove();
		void select(bool select = true) { selected = select; }
		bool isSelected() const { return selected; }

		void setVisible(bool nodeVisible = true) { visible = nodeVisible; };
		bool isVisible() const { return visible; }

		/**
		Invalida los volúmenes de inclusión del nodo y de sus predecesores
		*/
		void invalidateBoundingVolumes();

		void addUpdateCallback(std::shared_ptr<NodeCallback> nc);
		void removeUpdateCallback(std::shared_ptr<NodeCallback> nc);
		void clearUpdateCallbacks();
		const std::vector<std::shared_ptr<NodeCallback>> getUpdateCallbacks() const {
			return updateCallbacks;
		}

		uint32_t getId() const { return nodeId; }

	protected:
		void addParent(Group* parent);
		void removeParent(Group* parent);
		virtual void recomputeBoundingBox() = 0;
		virtual void recomputeBoundingSphere() = 0;
		std::vector<Group*> parents;
		std::string name;
		BoundingBox bb;
		BoundingSphere bs;
		bool selected;
		bool visible;
		std::vector<std::shared_ptr<NodeCallback>> updateCallbacks;
		uint32_t nodeId;
		friend class Group;
		friend class AnimationNode;
		static uint32_t nextNodeId;
	};
};

#endif

