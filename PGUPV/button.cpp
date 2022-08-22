#include "button.h"

#include <guipg.h>

using PGUPV::Button;

Button::Button(const std::string &label, std::function<void()> callback) : callback(callback), size(0.0f, 0.0f) {
	setLabel(label);
}

void Button::renderWidget() {
	if (GUILib::Buttom(label, size))
		callback();
}

void Button::setSize(float width, float height) {
	size.x = width;
	size.y = height;
}
