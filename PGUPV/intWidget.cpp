#include "uniformWriter.h"
#include "program.h"

#include "intWidget.h"

using PGUPV::IntWidget;
using PGUPV::Program;
using PGUPV::Value;

IntWidget::IntWidget(const std::string &label, int value, int v_min, int v_max) :
	min(v_min), max(v_max), value(value) {
	setLabel(label);
};

IntWidget::IntWidget(const std::string &label, int value, int min, int max,
	std::shared_ptr<Program> program, const std::string &uniform) : IntWidget(label, value, min, max) {
	this->program = program;
	GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
	auto writeUniform = [ul, program](int q) {
		auto prev = program->use();
		UniformWriter::write(ul, q);
		if (prev != nullptr) prev->use();
		else program->unUse();
	};
	writeUniform(value);
	this->value.addListener(writeUniform);
}


int IntWidget::get() const {
	return value.getValue();
}

void IntWidget::set(const int v, bool notifyListeners) {
	if (v < min) value.setValue(min, notifyListeners);
	else if (v > max) value.setValue(max, notifyListeners);
	else value.setValue(v, notifyListeners);
}

Value<int>& IntWidget::getValue() { return value; };

int IntWidget::getMin() const { return min; };
int IntWidget::getMax() const { return max; };

void IntWidget::setMin(int minimum, bool notifyListeners) {
	min = minimum;
	if (value.getValue() < minimum)
		value.setValue(minimum, notifyListeners);
}

void IntWidget::setMax(int maximum, bool notifyListeners) {
	max = maximum;
	if (value.getValue() > maximum)
		value.setValue(maximum, notifyListeners);
}
