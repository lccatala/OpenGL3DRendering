
#include <guipg.h>
//#include <glm/glm.hpp>

#include "program.h"
#include "uniformWriter.h"
#include "camera.h"

#include "directionWidget.h"

using PGUPV::DirectionWidget;
using PGUPV::Value;
using PGUPV::Program;


DirectionWidget::DirectionWidget(const std::string &label, const glm::vec3 &value) :
	value(value) {
	setLabel(label);
}

DirectionWidget::DirectionWidget(const std::string &label, const glm::vec3 &value,
	std::shared_ptr<Program> program, const std::string &uniform) :
	DirectionWidget(label, value) {
	this->program = program;
	GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
	auto writeUniform = [ul, program](glm::vec3 q) {
		program->use();
		glUniform3fv(ul, 1, &q.x);
	};
	writeUniform(value);
	this->value.addListener(writeUniform);
}

glm::vec3 DirectionWidget::get() const {
	return value.getValue();
}

void DirectionWidget::set(const glm::vec3 &d, bool notifyListeners) {
	value.setValue(d, notifyListeners);
}

Value<glm::vec3>& DirectionWidget::getValue() {
	return value;
};

void DirectionWidget::renderWidget() {
	auto original = value.getValue();
	if (cameraPtr) {
		auto xformed = glm::mat3(cameraPtr->getViewMatrix()) * original;
		if (GUILib::DirectionGizmo(label, xformed, original)) {
			auto result = glm::transpose(glm::mat3(cameraPtr->getViewMatrix())) * xformed;
			value.setValue(result);
		}
	}
	else {
		if (GUILib::DirectionGizmo(label, original)) {
			value.setValue(original);
		}
	}
}
