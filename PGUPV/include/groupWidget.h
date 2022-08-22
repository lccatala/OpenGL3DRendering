#pragma once
// 2019

#include <vector>
#include "widget.h"

namespace PGUPV {
	class GroupWidget : public Widget {
	public:
		GroupWidget() {};
		void addChild(std::shared_ptr<Widget> child) {
			children.push_back(child);
		}
		std::shared_ptr<Widget> getChild(unsigned int i) {
			return children[i];
		}

		void clearChildren() {
			children.clear();
		}
		void removeChildren(unsigned int child) {
			children.erase(children.begin() + child);
		}
		void renderWidget() override {
			for (auto &c : children) {
				c->render();
			}
		}
	protected:
		std::vector<std::shared_ptr<Widget>> children;
	};

};
