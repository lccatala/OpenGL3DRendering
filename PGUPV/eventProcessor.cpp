

#include "events.h"
#include "eventProcessor.h"
#include "eventSource.h"
#include "app.h"
#include "log.h"
#include "window.h"

using PGUPV::AppEventProcessor;
using PGUPV::SaveToStreamAndDispatchEventProcessor;
using PGUPV::Event;
using PGUPV::EventType;

void AppEventProcessor::dispatchPendingEvents() {
	Event e;
	while (!app.isDone() && app.getEventSource().getEvent(e)) {
		processEvent(e);
	}
}

void AppEventProcessor::processEvent(const PGUPV::Event &e) {
	switch (e.type) {
	case EventType::QuitEvent:
		app.done();
		break;
	case EventType::WindowResizedEvent:
		e.windowResized.wsrc->reshaped(e.windowResized.width, e.windowResized.height);
		break;
	case EventType::WindowClosedEvent:
		//e.windowResized.wsrc->destroy();
    //  TODO destruir una ventana no tendría por qué cerrar la aplicación
    app.done();
		break;
	case EventType::KeyboardEvent:
		app.onKeyboardEvent((KeyboardEvent &)e);
		break;
	case EventType::MouseButtonEvent:
		app.mouseButtonEvents.notify((MouseButtonEvent &)e);
		break;
	case EventType::MouseMotionEvent:
		app.mouseMotionEvents.notify((MouseMotionEvent &)e);
		break;
	case EventType::MouseWheelEvent:
		app.mouseWheelEvents.notify((MouseWheelEvent &)e);
		break;
	case EventType::JoystickMotionEvent:
		app.onJoyAxisMotionEvent((JoystickMotionEvent &)e);
		break;
	case EventType::JoystickButtonEvent:
		app.onJoyButtonEvent((JoystickButtonEvent &)e);
		break;
	case EventType::JoystickHatMotionEvent:
		app.onJoyHatMotionEvent((JoystickHatMotionEvent &)e);
		break;
	case EventType::TextInputEvent:
		// No use (yet)
		break;
	default:
		ERRT("Tipo de evento desconocido");
	}
}

void SaveToStreamAndDispatchEventProcessor::dispatchPendingEvents() {
	Event e;
	while (app.getEventSource().getEvent(e)) {
		output << e;
		processEvent(e);
	}
}

PGUPV::SaveToFileAndDispatchEventProcessor::SaveToFileAndDispatchEventProcessor(App & app, const std::string & filename)
	: SaveToStreamAndDispatchEventProcessor(app, file)
{
	file.open(filename);
}
