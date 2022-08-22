#include "label.h"
#include <guipg.h>

using PGUPV::Label;

Label::Label(const std::string &text) {
	label = text;
}
void Label::renderWidget() {
	GUILib::Text(label);
}

void Label::setText(const std::string &t) {
	label = t;
}
