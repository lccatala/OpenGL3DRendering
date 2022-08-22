#include <guipg.h>

#include "checkBoxWidget.h"
#include "program.h"

using PGUPV::CheckBoxWidget;
using PGUPV::Value;
using PGUPV::Program;

CheckBoxWidget::CheckBoxWidget(const std::string &label, const bool &value) : value(value) {
	setLabel(label);
}

CheckBoxWidget::CheckBoxWidget(const std::string &label, const bool &value, 
	std::shared_ptr<Program> program, const std::string &uniform) :
	CheckBoxWidget(label, value) {
	this->program = program;
	GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
	auto writeUniform = [ul, program](bool q) {
		program->use();
		glUniform1i(ul, q ? 1 : 0);
	};
	writeUniform(value);
	this->value.addListener(writeUniform);
}

bool CheckBoxWidget::get() const {
	return value.getValue();
}

void CheckBoxWidget::set(const bool q, bool notifyListeners) {
	value.setValue(q, notifyListeners);
}

Value<bool>& CheckBoxWidget::getValue() { 
	return value; 
};

void CheckBoxWidget::renderWidget() {
	auto b = value.getValue();
	if (GUILib::Checkbox(label, &b))
		value.setValue(b);
}
