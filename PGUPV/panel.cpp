#include "panel.h"

#include <guipg.h>

using PGUPV::Panel;
using PGUPV::Value;
using PGUPV::Widget;

Panel::Panel(const std::string &name) : name(name),
pos(glm::vec2(-1.f, -1.f)), size(glm::vec2(-1.f, -1.f)), visible(true)
{
};

Panel::~Panel() {
	LIBINFO("Panel " + name + " destruido");
};

void Panel::addWidget(std::shared_ptr<Widget> widget) {
	if (!widget) {
		ERRT("Has añadido al panel un widget vacío");
	}
	children.push_back(widget);
}

std::string Panel::getName() const {
	return name;
};

void Panel::setPosition(float x, float y) {
	pos = glm::vec2(x, y);
}

void Panel::setSize(float width, float height) {
	size = glm::vec2(width, height);
}

void Panel::render() {
	if (pos.x > 0.0F)
		GUILib::SetNextWindowPos(pos, GUILib::WindowPosSizeFlags::FirstUseEver);
	if (size.x > 0.0F)
		GUILib::SetNextWindowSize(size, GUILib::WindowPosSizeFlags::FirstUseEver);

	GUILib::Begin(name);
	for (auto &w : children)
		w->render();
	GUILib::End();
};

Value<bool> &Panel::getVisibleValue() {
	return visible;
}

void Panel::setVisible(bool panelVisible) {
	visible.setValue(panelVisible);
}

bool Panel::isVisible() const {
	return visible.getValue();
}
