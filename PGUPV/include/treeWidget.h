#pragma once
// 2017

#include "widget.h"
#include "value.h"
#include "treeNodeFlags.h"

#include <functional>
#include <vector>

namespace PGUPV {
	class Program;
	
	bool _treeNodeDraw(void *data, TreeNodeFlags flags, const std::string &name);
	bool _treeNodeIsItemClicked();
	void _treeNodeTreePop();


	template<typename T = void *>
	class TreeWidget : public Widget {
	public:

		struct TreeNode {
			TreeNode(const std::string &name) : name(name) {}
			TreeNode(const std::string &name, T userElement) : name(name), userElement(userElement) {}
			void addChild(std::shared_ptr<TreeNode> c) { children.push_back(c); }
			std::string name;
			T userElement;
			std::vector<std::shared_ptr<TreeNode>> children;
		};

		/**
		Constructor con los elementos de la lista
		\param label Título de la lista
		\param elements Lista de cadenas con los elementos de la lista
		\param selected índice del elemento seleccionado inicialmente (el primero por defecto)
		*/
		TreeWidget(const std::string &label, std::shared_ptr<TreeNode> elements = std::shared_ptr<TreeNode>()) :
			styler([](const T&) { return 0; })
		{
			LIBINFO("TreeWidget created: " + getName());
			setLabel(label);
			setElements(elements);
		}

		void setElements(std::shared_ptr<TreeNode> elems) {
			elements = elems;
		}

		void setNodeStyle(std::function<TreeNodeFlags(const T&)> nodeStyler) {
			styler = nodeStyler;
		}

		void renderWidget() override {
			if (!elements) return;
			showNode(elements);
		}

		Value<T> &getValue() {
			return value;
		}

		bool getSelected(T &selection) {
			if (selected) {
				selection = selected->userElement;
				return true;
			}
			return false;
		}
		void select(std::shared_ptr<TreeNode> node, bool notifyListeners = true) {
			selected = node;
			value.setValue(node->userElement, notifyListeners);
		}
	protected:
		void showNode(std::shared_ptr<TreeNode> node) {
			unsigned int node_flags =
				static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_OpenOnArrow) |
				static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_OpenOnDoubleClick) |
				static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_DefaultOpen) |
				(selected == node ? static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_Selected) : 0);
			node_flags |= styler(node->userElement);

			if (node->children.empty()) {
				// Leaf node
				_treeNodeDraw((void*)&(node->userElement), node_flags |
					static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_Leaf) |
					static_cast<unsigned int>(TreeNodeFlag::TreeNodeFlags_NoTreePushOnOpen), node->name);
				if (_treeNodeIsItemClicked()) {
					select(node);
				}
			}
			else {
				// Inner node
				bool node_open = _treeNodeDraw((void*)&(node->userElement), node_flags, node->name);
				if (_treeNodeIsItemClicked())
					select(node);
				if (node_open)
				{
					for (auto c : node->children) {
						showNode(c);
					}
					_treeNodeTreePop();
				}
			}
		}

		std::shared_ptr<TreeNode> elements;
		std::shared_ptr<TreeNode> selected;
		Value<T> value;
		std::function<TreeNodeFlags(const T&)> styler;
	};
};