#include <algorithm>

#include "keyboard.h"
#include "app.h"
#include "log.h"

using PGUPV::Keyboard;
using PGUPV::KeyCode;
using PGUPV::KeyboardEventsSource;

Keyboard::Keyboard() {
	App *instance = &App::getInstance();
	this->eventSource = &(instance->keyboardEvents);
	this->currentFrameFunc = [instance]() { return instance->getCurrentFrame(); };
	init();
}

Keyboard::Keyboard(KeyboardEventsSource *source, std::function<ulong()> currentFrameFunc) :
	eventSource(source), currentFrameFunc(currentFrameFunc) {
	init();
}

Keyboard::~Keyboard() {
	eventSource->removeListener(subscriptionId);
	eventSource = nullptr;
	keyboardState.clear();
}

void Keyboard::init() {
	subscriptionId = eventSource->addListener(std::bind(&Keyboard::onKeyboardEvent, this, std::placeholders::_1));
}

std::list<Keyboard::KeyEvent>::iterator Keyboard::findInListAndClean(KeyCode keycode) {
	auto i = keyboardState.begin();
	auto now = currentFrameFunc();
	while (i != keyboardState.end()) {
		// Limpiando pulsaciones pasadas
		if (i->tOut < now) {
			auto tmp = std::next(i);
			keyboardState.erase(i);
			i = tmp;
		}
		else {
			if (i->k == keycode ||
				(keycode == KeyCode::Control &&
				(i->k == KeyCode::LeftControl || i->k == KeyCode::RightControl)) ||
				(keycode == KeyCode::Shift &&
				(i->k == KeyCode::LeftShift || i -> k == KeyCode::RightShift)) ||
				(keycode == KeyCode::Alt &&
				(i->k == KeyCode::LeftAlt || i->k == KeyCode::RightAlt))
				)
				return i;
			i = std::next(i);
		}
	}
	return keyboardState.end();
}

void Keyboard::onKeyboardEvent(const PGUPV::KeyboardEvent &e) {
	// En cualquier caso, comprobar si estaba pulsada:
	auto it = findInListAndClean(e.key);
	ulong now = currentFrameFunc();
	if (e.state == ButtonState::Pressed) {
		// Se ha pulsado en este frame: almacenar
		if (it == keyboardState.end()) {
			KeyEvent n{ e.key, now, ULONG_MAX };
			keyboardState.push_back(n);
		}
		else {
			WARN("Esta tecla ya estaba pulsada");
			it->tIn = now;
		}
	}
	else {
		// Se ha soltado en este frame: almacenar en el de salida
		if (it == keyboardState.end()) {
			// Si no estaba pulsada, la ignoramos: no debería haber pasado
			WARN("Se ha soltado una tecla que no se había pulsado");
		}
		else {
			it->tOut = now;
		}
	}
}

bool Keyboard::isKeyDown(PGUPV::KeyCode k) {
	auto it = findInListAndClean(k);
	return it != keyboardState.end() &&
		it->tIn == currentFrameFunc();
}

bool Keyboard::isKeyPressed(PGUPV::KeyCode k) {
	/*
	El estado pressed ocurre entre el frame en el que se pulsa una tecla (incluído) y el
	frame en que se suelta (excluído, a no ser que se pulse y suelte en el mismo frame)
	*/
	auto it = findInListAndClean(k);
	ulong now = currentFrameFunc();
	return it != keyboardState.end() &&
		(it->tIn == now || now < it->tOut);
}

bool Keyboard::isKeyUp(PGUPV::KeyCode k) {
	auto it = findInListAndClean(k);
	return it != keyboardState.end() &&
		it->tOut == currentFrameFunc();
}
