#include <guipg.h>
#include "program.h"

#include "intSliderWidget.h"

using PGUPV::IntSliderWidget;
using PGUPV::Program;

IntSliderWidget::IntSliderWidget(const std::string &label, int value, int v_min, int v_max) :
	IntWidget(label, value, v_min, v_max), displayFormat("%.0f") {
};

IntSliderWidget::IntSliderWidget(const std::string &label, int value, int min, int max,
	std::shared_ptr<Program> program, const std::string &uniform) :
	IntWidget(label, value, min, max, program, uniform), displayFormat("%.0f") {
}

void IntSliderWidget::setDisplayFormat(const std::string &format) {
	displayFormat = format;
}
const std::string IntSliderWidget::getDisplayFormat() {
	return displayFormat;
}
void IntSliderWidget::renderWidget() {
	int i = value.getValue();

	if (GUILib::SliderInt(label, i, min, max, displayFormat))
		value.setValue(i);
}
