#include <guipg.h>
#include "program.h"
#include "rotationWidget.h"

using PGUPV::RotationWidget;
using PGUPV::Value;
using PGUPV::Program;


RotationWidget::RotationWidget(const std::string &label, const glm::mat4 &value)
	: value(glm::quat_cast(value)) {
	setLabel(label);
}

RotationWidget::RotationWidget(const std::string &label, const glm::mat4 &value, 
	std::shared_ptr<Program> program, const std::string &uniform)
	: RotationWidget(label, value)
{
	this->program = program;
	GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
	auto writeUniform = [ul, program](glm::quat q) {
		glm::mat4 tmp = glm::mat4_cast(q);
		program->use();
		glUniformMatrix4fv(ul, 1, GL_FALSE, &tmp[0][0]);
	};
	writeUniform(glm::quat_cast(value));
	this->value.addListener(writeUniform);
}

glm::quat RotationWidget::getQuaternion() const {
	return value.getValue();
}

glm::mat4 RotationWidget::get() const {
	return glm::mat4_cast(value.getValue());
}

void RotationWidget::setQuaternion(const glm::quat &q, bool notifyListeners) {
	value.setValue(q, notifyListeners);
}

void RotationWidget::set(const glm::mat4 &m, bool notifyListeners) {
	value.setValue(glm::quat_cast(m), notifyListeners);
}

void RotationWidget::set(const glm::mat3 &m, bool notifyListeners) {
	value.setValue(glm::quat_cast(m), notifyListeners);
}

Value<glm::quat>& RotationWidget::getValue() { 
	return value; 
};

void RotationWidget::renderWidget() {
	auto v = value.getValue();

	if (GUILib::QuaternionGizmo(label, v)) {
		value.setValue(v);
	}
}
