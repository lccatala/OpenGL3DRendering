#include "gamepad.h"
#include "log.h"
#include "utils.h"

using PGUPV::Gamepad;


// This translates an incoming axis into another, or discards it
// The first joystick has axes 0 and 1, the second 2 and 3, etc.
static const int axisRemapping[] = { 0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 8 };

Gamepad::Gamepad(size_t id) : gamepadHW(new GamepadHW(id)) {
	numButtons = gamepadHW->getNumButtons();
	size_t numAxes = gamepadHW->getNumAxes();
	for (size_t i = 0; i < numAxes / 2; i++)
		directions.push_back(glm::vec2(0.0f, 0.0f));
	for (size_t i = 0; i < gamepadHW->getNumHats(); i++)
		hats.push_back(PGUPV::Centered);
}

glm::vec2 Gamepad::getDirection(uint joystick) {
	return directions[joystick];
}

glm::vec2 Gamepad::getDirection() {
	for (size_t i = 0; i < directions.size(); i++) {
		if (directions[i].x != 0.0f || directions[i].y != 0.0f) {
			return directions[i];
		}
	}
	return glm::vec2(0.0f, 0.0f);
}

bool Gamepad::isPressed(uint button) {
	return buttons.test(button);
}

bool Gamepad::isPressed() {
	return buttons.any();
}

PGUPV::HatPosition Gamepad::getHatPosition(uint hat) {
	return hats[hat];
}

void Gamepad::onJoystickMove(const PGUPV::JoystickMotionEvent &e) {
	assert(e.axis <= directions.size() * 2);
	int remapped = axisRemapping[e.axis];
	if (remapped == -1) return;
	else {
		if (remapped % 2 == 0)
			directions[remapped / 2].x = e.value;
		else
			directions[remapped / 2].y = e.value;
		INFO(std::string("Joystick: ") + std::to_string(e.joystickId) + std::string(" Direction : ") + std::to_string(remapped / 2) + ": " + PGUPV::to_string(directions[remapped / 2]));
	}
}

void Gamepad::onHatMove(const PGUPV::JoystickHatMotionEvent &e) {
	assert(e.hatId < hats.size());
	hats[e.hatId] = e.position;
	INFO(std::string("Joystick: ") + std::to_string(e.joystickId) + std::string(" Hat : ") + std::to_string(e.hatId) + ": " + PGUPV::to_string(hats[e.hatId]));
}

void Gamepad::onJoystickButton(const PGUPV::JoystickButtonEvent &e) {
	assert(e.button < numButtons);
	buttons[e.button] = (e.state == ButtonState::Pressed);
	INFO(std::string("Joystick: ") + std::to_string(e.joystickId) + std::string(" Button : ") + std::to_string(e.button) + ": " + std::to_string(buttons.test(e.button)));
}

