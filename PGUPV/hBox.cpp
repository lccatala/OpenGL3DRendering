#include <guipg.h>

#include "hbox.h"

using PGUPV::HBox;

void HBox::addChild(std::shared_ptr<Widget> child) {
	children.push_back(child);
}

void HBox::renderWidget() {
	for (size_t i = 0; i < children.size() - 1; i++) {
		children[i]->render();
		GUILib::SameLine();
	}
	if (!children.empty()) {
		children.back()->render();
	}
}
