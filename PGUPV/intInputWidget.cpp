#include <guipg.h>
#include "program.h"

#include "intInputWidget.h"

using PGUPV::IntInputWidget;
using PGUPV::Program;

IntInputWidget::IntInputWidget(const std::string &label, int value, int v_min, int v_max) :
	IntWidget(label, value, v_min, v_max) {
};

IntInputWidget::IntInputWidget(const std::string &label, int value, int min, int max,
	std::shared_ptr<Program> program, const std::string &uniform) :
	IntWidget(label, value, min, max, program, uniform) {
}

void IntInputWidget::renderWidget() {
	int i = value.getValue();
	if (GUILib::InputInt(label, i)) {
		if (i < min) i = min;
		else if (i > max) i = max;
		value.setValue(i);
	}
}
