
#include <sstream>
#include <chrono>


#include <assimp/version.h>
#include <boost/version.hpp>

#include "app.h"
#include "eventProcessor.h"
#include "gamepad.h"
#include "statsClass.h"
#include "utils.h"
#include "commandLineProcessor.h"
#include "windowBuilder.h"
#include "log.h"
#include "renderer.h"
#include "keyboard.h"
#include "image.h"
#include "guipg.h"
#include "lifetimeManager.h"

using std::string;
using PGUPV::App;
using PGUPV::WindowBuilder;
using PGUPV::Window;
using PGUPV::ShaderLibrary;
using PGUPV::Log;
using PGUPV::Renderer;
using PGUPV::Keyboard;
using PGUPV::StatsClass;
using PGUPV::StopWatch;
using PGUPV::CommandLineProcessor;
using PGUPV::GLVersion;
using PGUPV::EventSource;
using PGUPV::EventProcessor;

#define DEFAULT_MAJOR_GL_VERSION 4

#ifdef __APPLE__
#define DEFAULT_MINOR_GL_VERSION 1
#else
#define DEFAULT_MINOR_GL_VERSION 5
#endif

#define DEFAULT_MINIMUM_MINOR_GL_VERSION 0


#define PROPERTIES_FILE_NAME "properties.ini"

const char *App::PROP_HIGHEST_GL_VERSION_SEEN = "highest_gl_version_seen";
const char *App::PROP_FFMPEG_EXEC_PATH = "ffmpeg_exec_path";

unsigned int App::_elapsed = 0;

#ifdef _MSC_VER 
extern "C" {
	// In laptops, request the discrete GPU instead of the integrated GPU
	// https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	// https://gpuopen.com/amdpowerxpressrequesthighperformance/
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

App &App::getInstance(void) {
	static App *_appinstance = nullptr;
	if (_appinstance == nullptr) {
		_appinstance = new App();
		SetLongevity(_appinstance, 10, Private::Deleter<App>::Delete);
	}
	return *_appinstance;
}

App::App()
	: _errorCode(0), _appDone(false), _paused(false), _show_fps(false), _take_snapshot(false), _destroyed(false),
	_current_frame(0U), _running_time(0.0), initX(50U), initY(50), initWidth(800U), initHeight(600U),
	ftl(-1), preferredMajorGLVer(DEFAULT_MAJOR_GL_VERSION), preferredMinorGLVer(-1),
	minimumGLVer(DEFAULT_MINIMUM_MINOR_GL_VERSION), stats(std::make_shared<StatsClass>()),
	eventSource(std::unique_ptr<EventSource>(new EventSourceHW())),
	eventProcessor(std::unique_ptr<EventProcessor>(new AppEventProcessor(*this))),
	ignoreCameras(false), ignoreGUIState(false)
{
	// Con esto nos aseguramos que Log sea el primer objeto estático en crearse (después de App), y
	// que sea el penúltimo en destruírse
	INFO("App created");

	if (!properties.load(PROPERTIES_FILE_NAME))
		properties.saveAs(PROPERTIES_FILE_NAME);
}

App::~App() {
	INFO("App destroyed");
}

PGUPV::Window &App::getWindow(uint index) const {
	if (m_windows.size() <= index) {
		ERRT("No se ha creado la ventana de índice" + std::to_string(index));
	}
	return *m_windows[index];
}

bool App::initApp(CommandLineProcessor cmdLine, uint flags) {
	cmdLine.process(*this);

	INFO("Iniciando aplicación " + cmdLine.getExecutableName());

	hw = std::unique_ptr<HW>(new HW());

	keyboardCache = std::unique_ptr<Keyboard>(new Keyboard());


	// When the user does not set a preferred GL version...
	if (preferredMinorGLVer == -1) {
		// We try to use the highest seen
		std::string highest_seen;
		if (getProperty(App::PROP_HIGHEST_GL_VERSION_SEEN, highest_seen)) {
			auto highgl = GLVersion::parse(highest_seen);
			preferredMinorGLVer = highgl.minor;
		}
		else {
			preferredMinorGLVer = DEFAULT_MINOR_GL_VERSION;
		}
	}

	// Last state of the window saved in the properties file
	getProperty("window-pos-x", initX);
	getProperty("window-pos-y", initY);
	getProperty("window-width", initWidth);
	getProperty("window-height", initHeight);

	WindowBuilder wb;
	Window *w =
		wb.setTitle("PGUPV")
		.setFlags(flags)
		.setInitPosition(initX, initY)
		.setInitSize(initWidth, initHeight)
		.setMinOpenGLVer(minimumGLVer)
		.setPrefOpenGLVer(preferredMajorGLVer, preferredMinorGLVer)
		.build();

	if (w == NULL) {
		ERRT("No se ha podido construir la ventana");
	}

	m_windows.push_back(w);

	CHECK_GL();

	// we store the highest version that worked
	GLVersion to_save = w->getOpenGLVersion();
	std::string highest_seen;
	if (getProperty(App::PROP_HIGHEST_GL_VERSION_SEEN, highest_seen)) {
		GLVersion prev = GLVersion::parse(highest_seen);
		if (prev.isGreaterEqual(to_save))
			to_save = prev;
	}
	setProperty(PROP_HIGHEST_GL_VERSION_SEEN, to_save.to_string());


	INFO(collectLibVersions());

	initJoysticks();

	// Register callbacks
	joyMotionSubsId = joystickAxisMotionEvent.addListener(std::bind(&App::onJoyAxisMotionEvent, this, std::placeholders::_1));
	joyHatSubsId = joystickHatMotionEvent.addListener(std::bind(&App::onJoyHatMotionEvent, this, std::placeholders::_1));
	joyButtonSubsId = joystickButtonEvent.addListener(std::bind(&App::onJoyButtonEvent, this, std::placeholders::_1));

	return true;
}

bool App::initApp(int argc, char **argv, uint flags) {
	CommandLineProcessor cmdLine(argc, argv);
	return initApp(cmdLine, flags);
}

std::string App::collectLibVersions() {
	std::ostringstream vs;

	vs << "Library versions\nPGUPV";

	constexpr bool sixtyFour{ sizeof(void *) == 8 };
	if (sixtyFour)
		vs << " 64 bits";
	else
		vs << " 32 bits";

	vs
#ifdef _DEBUG
		<< " Debug"
#else 
		<< " Release"
#endif
#ifdef TESTING
		<< " for Testing"
#endif
		<< " Revision: 2158"
		"\n";

	// Boost version
	vs << "Boost: " BOOST_LIB_VERSION  "\n";

	// HW (SDL)
	vs << hw->getLibVersionString() << "\n";

	// Assimp version
	vs << "Assimp: " <<
		aiGetVersionMajor() << "." <<
		aiGetVersionMinor() << "." <<
		aiGetVersionRevision() << "\n";

	// GLEW version
	vs << "GLEW: " << (char *)glewGetString(GLEW_VERSION) << "\n";

	// Par Shapes
	vs << "Par Shapes: http://github.prideout.net/shapes @ August 2016" << "\n";

	// GLI & FreeImage
	vs << PGUPV::Image::getLibraryInfo() << "\n";

	// GUI library
	vs << GUILib::getVersion() << "\n";

	// GLM
	vs << GLM_VERSION_MESSAGE << "\n";

	return vs.str();
}

void App::onJoyAxisMotionEvent(const PGUPV::JoystickMotionEvent &e) {
	assert(e.joystickId < gameControllers.size());
	gameControllers[e.joystickId]->onJoystickMove(e);
}


void App::onJoyHatMotionEvent(const PGUPV::JoystickHatMotionEvent &e) {
	assert(e.joystickId > 0 && e.joystickId < (int)gameControllers.size());
	gameControllers[e.joystickId]->onHatMove(e);
}

void App::onJoyButtonEvent(const PGUPV::JoystickButtonEvent &e) {
	assert(e.joystickId > 0 && e.joystickId < (int)gameControllers.size());
	gameControllers[e.joystickId]->onJoystickButton(e);
}


std::string App::getShortcutsHelp() {
	std::stringstream o;

	o << "Teclas disponibles:\n";
	o << "  Ctrl + h: muestra esta ayuda\n";
	o << "  Ctrl + 1, 2, 3: mostrar el buffer de color, profundidad y stencil\n";
	o << "  Ctrl + s: capturar el contenido de la ventana a fichero\n";
	o << "  Ctrl + f: pantalla completa\n";
	o << "  Ctrl + b: activar/desactivar culling\n";
	o << "  Ctrl + i: información sobre el estado de OpenGL\n";
	o << "  Ctrl + Mays + i: aún más información sobre OpenGL\n";
	o << "  Ctrl + g: mostrar/ocultar el GUI\n";
	o << "  Ctrl + x: mostrar/ocultar estadísticas\n";
	o << "  Ctrl + c: mostrar/ocultar la consola de log\n";
	o << "  Ctrl + m: modo de dibujo de polígonos (sólido, aristas, vértices)\n";
	o << "  Ctrl + space: liberar/capturar el ratón de la cámara\n";
	o << "  Ctrl + n: modo de inspección de la escena\n";
	o << "  Ctrl + F? / F? : guardar / cargar posición de la cámara (teclas de función)\n";
	o << "  ESC: terminar el programa\n";
	o << "  P: pausa";

	return o.str();
}

void App::onKeyboardEvent(const PGUPV::KeyboardEvent &e) {
	// Reserved shortcuts (not received by the window):
	// Ctrl + 1, 2, 3: show color, depth and stencil buffer
	// Ctrl + s: take a snapshot of the currently shown buffer
	// Ctrl + f: full screen
	// Ctrl + i: info about the OpenGL status
	// Ctrl + g: toggle GUI visibility
	// Ctrl + m: cycle polygon drawing modes
	// Ctrl + space: tell the camera handler to release/capture the mouse
	// Ctrl + b: toggle back face culling
	// Ctrl + n: toggle inspection camera
	// Ctrl + h: show onscreen help
	// ESC: end program
	// P: pause
	if (e.mod & PMOD_CTRL) {
		// Ctrl + <something>
		switch (e.key) {
		case KeyCode::S:
			if (e.state == ButtonState::Pressed)
				snapshot();
			break;
		case KeyCode::Alpha1:
			if (e.state == ButtonState::Pressed)
				e.wsrc->showBuffer(Window::COLOR_BUFFER);
			break;
		case KeyCode::Alpha2:
			if (e.state == ButtonState::Pressed)
				e.wsrc->showBuffer(Window::DEPTH_BUFFER);
			break;
		case KeyCode::Alpha3:
			if (e.state == ButtonState::Pressed)
				e.wsrc->showBuffer(Window::STENCIL_BUFFER);
			break;
		case KeyCode::F:
			if (e.state == ButtonState::Pressed)
				e.wsrc->setFullScreen(!e.wsrc->isFullScreenSet());
			break;
		case KeyCode::G:
			if (e.state == ButtonState::Pressed)
				e.wsrc->showGUI(!e.wsrc->isGUIVisible());
			break;
		case KeyCode::I:
			if (e.state == ButtonState::Pressed) {
				std::ostringstream os;
				shaderLib.printInfoShaders(os, (e.mod & PMOD_SHIFT) != 0);
				NOTIFICATION_LEVEL oldnl =
					Log::getInstance().getNotificationLevel();
				Log::getInstance().setNotificationLevel(INFO_LEVEL);
				INFO(os.str());
				Log::getInstance().setNotificationLevel(oldnl);
			}
			break;
		case KeyCode::M:
			if (e.state == ButtonState::Pressed) {

				// El siguiente código debería funcionar, según la especficación de
				// OpenGL 4.5 (ver tabla 23.10), pero AMD y MAC no reconocen la
				// constante GL_POLYGON_MODE. Por ello, he introducido en hacks.h
				// una sustitución a las llamadas a glPolygonMode para almacenar
				// en una variable privada el último estado establecido. Hago lo
				// mismo en glGetIntegerv para capturar la consulta a
				// GL_POLYGON_MODE
				GLint pm[2];
				glGetIntegerv(GL_POLYGON_MODE, pm);
				if (pm[0] == GL_FILL)
					pm[0] = GL_LINE;
				else if (pm[0] == GL_LINE)
					pm[0] = GL_POINT;
				else if (pm[0] == GL_POINT)
					pm[0] = GL_FILL;
				else
					ERR("Consulta de GL_POLYGON_MODE no soportada");
				glPolygonMode(GL_FRONT_AND_BACK, pm[0]);
			}
			break;
		case KeyCode::B:
			if (e.state == ButtonState::Pressed) {
				if (glIsEnabled(GL_CULL_FACE))
					glDisable(GL_CULL_FACE);
				else
					glEnable(GL_CULL_FACE);
			}
			break;
		case KeyCode::Space:
			if (e.state == ButtonState::Pressed) {
				if (e.wsrc) {
					std::shared_ptr<CameraHandler> c = e.wsrc->getRenderer()->getCameraHandler();
					if (c != nullptr) {
						if (c->isMouseCaptured())
							c->releaseMouse();
						else
							c->captureMouse();
					}
				}
			}
			break;
		case KeyCode::N:
			if (e.state == ButtonState::Released) {
				if (e.wsrc) e.wsrc->getRenderer()->useInspectionCamera(!e.wsrc->getRenderer()->isUsingInspectionCamera());
			}
			break;
		case KeyCode::H:
			if (e.state == ButtonState::Released) {
				if (e.wsrc) e.wsrc->showHelp(!e.wsrc->isHelpVisible());
			}
			break;
		case KeyCode::X:
			if (e.state == ButtonState::Released) {
				if (e.wsrc) e.wsrc->showStats(!e.wsrc->isStatsOpen());
			}
			break;
		case KeyCode::C:
			if (e.state == ButtonState::Released) {
				if (e.wsrc) e.wsrc->showConsole(!e.wsrc->isConsoleVisible());
			}
			break;
		default:
			if (e.state == ButtonState::Released && isFunctionKey(e.key)) {
				saveAppStatus(static_cast<int>(e.key) - static_cast<int>(KeyCode::F1));
			}
			// It is Ctrl+something, but we don't know what to do about it. Pass it on
			keyboardEvents.notify(e);
			break;
		}
	}
	else if (e.key == KeyCode::Escape) {
		if (e.state == ButtonState::Pressed) {
			if (e.wsrc && e.wsrc->isHelpVisible())
				e.wsrc->showHelp(false);
			else
				done();
		}
	}
	else if (e.key == KeyCode::P) {
		if (e.state == ButtonState::Pressed)
			pause(!isPaused());
	}
	else if (e.state == ButtonState::Released && isFunctionKey(e.key)) {
		loadAppStatus(static_cast<int>(e.key) - static_cast<int>(KeyCode::F1));
	}
	else

		keyboardEvents.notify(e);
}

void App::initJoysticks() {
	//Check for joysticks 
	size_t nJoysticks = hw->getNumGamepads();
	if (nJoysticks < 1) {
		INFO("No se han encontrado joysticks en el sistema");
	}
	else {
		INFO("Se han encontrado " + std::to_string(nJoysticks) + " joystick(s).");
		//Load joystick 
		for (size_t i = 0; i < nJoysticks; i++) {
			gameControllers.push_back(std::unique_ptr<Gamepad>(new Gamepad(i)));
		}
	}
}

size_t App::getNumGamepads() {
	return gameControllers.size();
}

void App::onShutdown(std::function<void()> f) {
	onShutdownFunctions.push_back(f);
}

void App::destroy() {
	LIBINFO("App::destroy()");

	// Se ha inicializado antes de su destrucción?
	if (!hw)
		return;


	joystickAxisMotionEvent.removeListener(joyMotionSubsId);
	joystickButtonEvent.removeListener(joyButtonSubsId);
	joystickHatMotionEvent.removeListener(joyHatSubsId);

	for (auto f = onShutdownFunctions.rbegin(); f != onShutdownFunctions.rend(); f++) {
		(*f)();
	}

	onShutdownFunctions.clear();


	preRenderCallbacks.clear();
	postRenderCallbacks.clear();

	int px, py;
	getWindow().getWindowHW()->getWindowPos(px, py);

	setProperty("window-pos-x", std::to_string(px));
	setProperty("window-pos-y", std::to_string(py));

	setProperty("window-width", std::to_string(getWindow().width()));
	setProperty("window-height", std::to_string(getWindow().height()));

	getWindow().destroy();

	for (auto w : m_windows)
		SDELETE(w);

	gameControllers.clear();

	hw->terminate();
	hw.reset();

	_destroyed = true;

}

uint App::getCurrentMillis() {
	return hw->currentMillis();
}

int64_t App::getCurrentMicroSecs() {
	auto nowSinceEpoch = std::chrono::system_clock::now().time_since_epoch();
	int64_t ns = std::chrono::duration_cast<std::chrono::microseconds>(nowSinceEpoch).count();
	return ns;
}

double App::getAppTime() { return _running_time; }


void App::setStatsObj(std::shared_ptr<StatsClass> statsObject) {
	getInstance().stats = statsObject;
}

void App::setEventProcessor(std::unique_ptr<EventProcessor> eventProcessor) {
	getInstance().eventProcessor = std::move(eventProcessor);
}

/**
 Devuelve el objeto receptor de estadísticas
 */
std::shared_ptr<StatsClass> App::getStatsObj() {
	return getInstance().stats;
}


int App::showMessageDialog(PGUPV::DialogType type, const std::string &title, const std::string &message) {
	return hw->showMessageBox(type, title, message);
}


int App::getWindowIndex(const Window * w) const
{
	for (uint i = 0; i < m_windows.size(); i++)
		if (w == m_windows[i])
			return static_cast<int>(i);
	return -1;
}

void App::processEvents() {
	auto sw = stats->makeStopWatch();
	eventProcessor->dispatchPendingEvents();
	stats->pushValue(std::to_string(sw->getElapsed()));
}

void App::update(uint elapsedMs) {
	_running_time += elapsedMs / 1000.0;
	_elapsed = elapsedMs;
	if (elapsedMs > 0) {
		auto sw = stats->makeStopWatch();
		for (auto w : m_windows)
			w->update(elapsedMs);
		stats->pushValue(std::to_string(sw->getElapsed()));
	}
}

void App::render() {
	auto sw = stats->makeStopWatch();
	for (auto p : preRenderCallbacks) {
		p.second();
	}
	// TODO: si hay varias ventanas, habría que cambiar el contexto aquí y dibujar cada una en orden
	m_windows[0]->draw();
	stats->pushValue(std::to_string(sw->getElapsedAndRestart()));
	for (auto p : postRenderCallbacks) {
		p.second();
	}
	m_windows[0]->swapBuffers();
	stats->pushValue(std::to_string(sw->getElapsed()));
}

int App::run() {
	try {
		_running_time = 0.0;
		uint _last_tick = hw->currentMillis();
		_current_frame = 0;
		//for (auto w : m_windows)
		//  w->reshaped(w->width(), w->height());
		stats->pushValue("Frame #").pushValue("Events (us)").pushValue("Update (us)").pushValue("Client Render (us)")
			.pushValue("GUI Render (us)").pushValue("Swap buffers (us)").pushValue("Total (us)").endFrame();
		auto frameStopWatch = stats->makeStopWatch();
		while (!_appDone) {
			FRAME("Empezando a dibujar el frame " + std::to_string(_current_frame));
			stats->pushValue(std::to_string(_current_frame));
			frameStopWatch->restart();
			processEvents();
			uint now = hw->currentMillis();
			if (!_paused) {
				uint elapsed = now - _last_tick;
				update(elapsed);
			}
			_last_tick = now;
			render();
			if (_take_snapshot) {
				takeSnapshot();
				_take_snapshot = false;
			}
			if (snapshots.popValue(_current_frame)) {
				// TODO ¿qué pasa cuando hay varias ventanas?
				m_windows[0]->saveColorBuffer(buildFrameName("frame", _current_frame));
			}
			stats->pushValue(std::to_string(frameStopWatch->getElapsed())).endFrame();
			if (ftl == static_cast<int64_t>(_current_frame)) {
				return 0;
			}
			_current_frame++;
			FRAME("Frame terminado");
			hw->sleep(1);
		}
	}
	catch (std::exception &) {
		std::cerr << "Excepcion no manejada. Más información en el fichero log.txt." << std::endl;
		return -1;
	}
	INFO("Aplicación terminada normalmente");

	destroy();

	return 0;
}

void App::takeSnapshot() {
	// TODO: ¿Qué pasa cuando hay varias ventanas?
	switch (m_windows[0]->getShownBuffer()) {
	case Window::COLOR_BUFFER:
		m_windows[0]->saveColorBuffer("color" + getTimeStamp() + ".png");
		break;
	case Window::DEPTH_BUFFER:
		m_windows[0]->saveColorBuffer("depth" + getTimeStamp() + ".png");
		break;
	case Window::STENCIL_BUFFER:
		m_windows[0]->saveColorBuffer("stencil" + getTimeStamp() + ".png");
		break;
	}
}


void App::setInitWindowSize(int width, int height) {
	// TODO: ¿Qué pasa cuando hay varias ventanas?
	if (!m_windows.empty()) {
		ERRT("Tienes que establecer el tamaño inicial de la ventana antes de "
			"llamar a initApp");
	}
	initHeight = height;
	initWidth = width;
}

void App::setInitWindowPos(int x, int y) {
	// TODO: ¿Qué pasa cuando hay varias ventanas?
	if (!m_windows.empty()) {
		ERRT("Tienes que establecer la posición inicial de la ventana antes de "
			"llamar a initApp");
	}
	initX = x;
	initY = y;
}

void App::setFramesToLive(long frames) { ftl = frames; }

void App::captureSnapshots(PGUPV::Intervals ints) {
	snapshots.addIntervals(ints);
}

void App::setMinimumGLVersion(uint minor) {
	minimumGLVer = minor;
}


void App::setPrefGLVersion(uint major, uint minor) {
	preferredMajorGLVer = major;
	preferredMinorGLVer = minor;
}

GLint App::getScratchUnitTextureNumber() {
	static GLint scratchUnitTexture = -1;
	if (scratchUnitTexture == -1) {
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &scratchUnitTexture);
		// Nos quedamos con la última unidad de textura
		scratchUnitTexture--;
		INFO("Unidad de textura reservada para PGUPV: " +
			std::to_string(scratchUnitTexture));
	}
	return scratchUnitTexture;
}


bool App::isKeyDown(PGUPV::KeyCode k) {
	return getInstance().keyboardCache->isKeyDown(k);
}

bool App::isKeyPressed(PGUPV::KeyCode k) {
	return getInstance().keyboardCache->isKeyPressed(k);
}

bool App::isKeyUp(PGUPV::KeyCode k) {
	return getInstance().keyboardCache->isKeyUp(k);
}

ShaderLibrary & App::getShaderLibrary() {
	return getInstance().shaderLib;
}

void App::setProperty(const std::string &name, const std::string &value) {
	properties.setValue(name, value);
	properties.save();
}

bool App::removeProperty(const std::string &name) {
	return properties.remove(name);
}

void App::setEventSource(std::unique_ptr<EventSource> evSource) {
	this->eventSource = std::move(evSource);
}

size_t App::addPreRender(std::function<void()> callback) {
	size_t id = preRenderCallbacks.size() + 1;
	preRenderCallbacks[id] = callback;
	INFO("Callback preRender instalada: " + std::to_string(id));
	return id;
}

void App::removePreRender(int id) {
	preRenderCallbacks.erase(id);
	INFO("Callback preRender desinstalada: " + std::to_string(id));
}

size_t App::addPostRender(std::function<void()> callback) {
	size_t id = postRenderCallbacks.size() + 1;
	postRenderCallbacks[id] = callback;
	INFO("Callback postRender instalada: " + std::to_string(id));
	return id;
}

void App::removePostRender(size_t id) {
	postRenderCallbacks.erase(id);
	INFO("Callback postRender desinstalada: " + std::to_string(id));
}


void App::saveAppStatus(int idx) {
	std::ofstream file("camdef" + std::to_string(idx) + ".txt");
	auto renderer = getWindow(0).getRenderer();
	renderer->getCameraHandler()->saveStatus(file);
	renderer->saveStatus(file);
}


void App::loadAppStatus(int idx) {
	std::string filename("camdef" + std::to_string(idx) + ".txt");
	std::ifstream file(filename);
	if (!file) {
		ERR("No se ha podido cargar el fichero " + filename);
	}
	else {
		auto renderer = getWindow(0).getRenderer();
		renderer->getCameraHandler()->loadStatus(file);
		renderer->loadStatus(file);
	}
}
