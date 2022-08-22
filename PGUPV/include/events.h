#ifndef _EVENTS_H
#define _EVENTS_H 2011

#include "common.h"
#include "observable.h"

namespace PGUPV {

	class Window;

	enum class EventType {
		KeyboardEvent, MouseMotionEvent, MouseButtonEvent, MouseWheelEvent,
		JoystickMotionEvent, JoystickHatMotionEvent, JoystickButtonEvent, WindowResizedEvent,
		WindowClosedEvent, QuitEvent, TextInputEvent
	};

	enum class ButtonState { Released, Pressed };
	enum {
		PMOD_NONE = 0x0000,
		PMOD_LSHIFT = 0x0001,
		PMOD_RSHIFT = 0x0002,
		PMOD_LCTRL = 0x0040,
		PMOD_RCTRL = 0x0080,
		PMOD_LALT = 0x0100,
		PMOD_RALT = 0x0200,
		PMOD_LGUI = 0x0400,
		PMOD_RGUI = 0x0800,
		PMOD_NUM = 0x1000,
		PMOD_CAPS = 0x2000,
		PMOD_MODE = 0x4000,
		PMOD_RESERVED = 0x8000,
	};
#define PMOD_CTRL  (PMOD_LCTRL|PMOD_RCTRL)
#define PMOD_SHIFT  (PMOD_LSHIFT|PMOD_RSHIFT)
#define PMOD_ALT  (PMOD_LALT|PMOD_RALT)
#define PMOD_GUI  (PMOD_LGUI|PMOD_RGUI)


#define MOUSE_LEFT_BUTTON 1
#define MOUSE_CENTER_BUTTON 2
#define MOUSE_RIGHT_BUTTON 4
#define MOUSE_SIDE_BUTTON1 8
#define MOUSE_SIDE_BUTTON2 16



	struct MouseMotionEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
		uchar state;        /**< The current button state, in a bit mask */
		int x;              /**< X coordinate, relative to window */
		int y;              /**< Y coordinate, relative to window */
		int xrel;           /**< The relative motion in the X direction */
		int yrel;           /**< The relative motion in the Y direction */
	};

	struct MouseButtonEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
		uchar button;       /**< The mouse button index: 1 left, 2: center, 3: right, 4: side... */
		ButtonState state;        /**< RELEASED, PRESSED */
		int x;              /**< X coordinate, relative to window */
		int y;              /**< Y coordinate, relative to window */
	};

	struct MouseWheelEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
		int x;              /**< The amount scrolled horizontally */
		int y;              /**< The amount scrolled vertically */
	};

	enum class KeyCode {
		Unknown, Return, Escape, BackSpace, Tab, Space, Exclamation, DblQuotes, Hash,
		Percent, Dollar, Ampersand, Quote, LeftParen, RightParen, Asterisk,
		Plus, Comma, Minus, Period, Slash, Zero, Alpha0, Alpha1, Alpha2, Alpha3, Alpha4,
		Alpha5, Alpha6, Alpha7, Alpha8, Alpha9, Colon, Semicolon, Less, Greater, Equals,
		Question, At, LeftBracket /* [ */, BackSlash, RightBracket /* ] */,
		Caret /* ^ */, Underscore, BackQuote /* ` */,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		CapsLock, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		PrintScreen, ScrollLock, Pause, Insert, Home, PageUp, Delete, End, PageDown,
		Right, Left, Down, Up, LeftControl, RightControl, LeftShift, RightShift,
		LeftAlt, RightAlt, Control, Shift, Alt,
		// Keypad keys
		NumLock, KeypadSlash, KeypadAsterisk, KeypadMinus, KeypadPlus, KeypadEnter,
		Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8,
		Keypad9, Keypad0, KeypadPeriod
	};

	bool isFunctionKey(KeyCode code);

	struct KeyboardEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
		ButtonState state;		/* RELEASED, PRESSED */
		KeyCode key;			/* La tecla pulsada o soltada */
		ushort mod;			/* PMOD_CTRL, PMOD_LSHIFT... o combinación con OR */
	};



	struct JoystickMotionEvent {
		EventType type;     // Joysticks has no window associated
		unsigned int joystickId;  // Which joystick has generated the event
		unsigned int axis;		// Which axis generated the event
		float value;		// Position of the pad in the axis (normalized between -1 and 1)
	};

	enum HatPosition {
		LeftUp, Up, RightUp,
		Left, Centered, Right,
		LeftDown, Down, RightDown
	};

	struct JoystickHatMotionEvent {
		EventType type;     // Joysticks has no window associated
		int joystickId;  // Which joystick generated the event
		uint hatId;		// Which hat generated the event
		HatPosition position; // Current position of the hat
	};

	struct JoystickButtonEvent {
		EventType type;     // Joysticks has no window associated
		int joystickId;  // Which joystick has generated the event
		uchar button;       /**< The joystick button index */
		ButtonState state;        /**< RELEASED, PRESSED */
	};

	struct WindowResizedEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
		uint width, height;
	};

	struct WindowClosedEvent {
		EventType type;
		Window *wsrc; // The window that generated the event
	};

	struct QuitEvent {
		EventType type;
	};

	struct TextInputEvent {
		EventType type;
		Window *wsrc;
		char text[32];
	};

	union Event {
		EventType type;
		MouseMotionEvent mouseMotion;
		MouseButtonEvent mouseButton;
		MouseWheelEvent mouseWheel;
		KeyboardEvent keyboard;
		JoystickMotionEvent joystickMotion;
		JoystickHatMotionEvent joystickHatMotion;
		JoystickButtonEvent joystickButton;
		WindowResizedEvent windowResized;
		WindowClosedEvent windowClosed;
		QuitEvent quit;
		TextInputEvent textInput;

		friend std::ostream& operator<<(std::ostream& os, const Event& e);
	};

	std::ostream& operator<<(std::ostream& os, const Event& e);

	/**
	Devuelve el próximo evento del frame indicado.
	\param is flujo de lectura
	\param frame número de frame en el que se produjo el evento original
	\param result donde se escribirá el resultado
	\return true si hay un evento para el frame indicado
	*/
	bool readNextEventForFrame(std::istream &is, long frame, Event &result);

	typedef Observable<KeyboardEvent> KeyboardEventsSource;
	typedef Observable<MouseMotionEvent> MouseMotionEventsSource;
	typedef Observable<MouseButtonEvent> MouseButtonEventsSource;
	typedef Observable<MouseWheelEvent> MouseWheelEventsSource;
	typedef Observable<JoystickMotionEvent> JoystickAxisMotionEventsSource;
	typedef Observable<JoystickHatMotionEvent> JoystickHatMotionEventsSource;
	typedef Observable<JoystickButtonEvent> JoystickButtonEventsSource;
};

#endif
