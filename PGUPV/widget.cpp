#include "widget.h"
#include "guipg.h"


using PGUPV::Widget;

unsigned long Widget::counter = 0;

void Widget::render() {
	if (visible) {
		bool wasDisabled = disabled;
		if (wasDisabled) {
			GUILib::StartDisabledWidgets();
		}
		if (preferredWidth >= 0.0f) {
			GUILib::NextItemGrowWidth(preferredWidth);
		}
		renderWidget();
		if (wasDisabled) {
			GUILib::StopDisabledWidgets();
		}
	}
}

void Widget::setWidth(float pct)
{
	preferredWidth = pct;
}
