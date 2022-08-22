
#include <stdexcept>
#include <sstream>



#include "app.h"
#include "window.h"
#include "image.h"
#include "HW.h"
#include "panel.h"
#include "bufferRenderer.h"
#include "textOverlay.h"
#include "glStateCache.h"
#include "GUI3.h"
#include "utils.h"
#include "logConsole.h"
#include "guipg.h"

using PGUPV::Window;
using PGUPV::Renderer;
using PGUPV::CameraHandler;
using PGUPV::Image;
using PGUPV::TextureRectangle;
using PGUPV::GLVersion;

bool Window::_glewReady = false;


Window::Window()
	: _openGLReqMaj(0), _openGLReqMin(0), _openGLCompatibility(false),
	_showfps(false), _showHelp(false), _showConsole(false), _showBuffer(COLOR_BUFFER), _showGUI(false),
	_openGLVersion(0, 0, 0),
	_redBits(0), _greenBits(0), _blueBits(0), _alphaBits(0), _depthBits(0),
	_stencilBits(0), console(new PGUPV::LogConsole()) {

	if (App::getInstance().isForgetGUIState()) {
		GUILib::forgetState();
	}

	consoleSubs = PGUPV::Log::getInstance().addListener([&](const std::string &msg) {console->add(msg); });
}

Window::~Window() {
	assert(renderers.size() == 0);
	PGUPV::Log::getInstance().removeListener(consoleSubs);
}

void Window::destroy() {
	deregisterEventHandlers();
	renderers.clear();
	window.reset();
}

void Window::printInfo(std::ostream &os) {
	const char *v;
	os << "GL_VENDOR: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_VENDOR))) != nullptr
			? v
			: "Could not obtain GL_VENDOR") << std::endl;
	os << "GL_RENDERER: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_RENDERER))) != nullptr
			? v
			: "Could not obtain GL_RENDERER") << std::endl;
	os << "GL_VERSION: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_VERSION))) != nullptr
			? v
			: "Could not obtain GL_VERSION") << std::endl;
	os << "GL_SHADING_LANGUAGE_VERSION: "
		<< ((v = reinterpret_cast<const char *>(
			glGetString(GL_SHADING_LANGUAGE_VERSION))) != nullptr
			? v
			: "Could not obtain GL_SHADING_LANGUAGE_VERSION") << std::endl;

	os << "Tamaño del framebuffer por defecto:" << std::endl;
	os << "   bits de color: (" << _redBits << ", " << _greenBits << ", "
		<< _blueBits << ", " << _alphaBits << "), depth: " << _depthBits
		<< ", stencil: " << _stencilBits << std::endl;
}

// Declara la versión de OpenGL que necesita la aplicación. Llamar a esta
// función *antes* de
// a createWindow y comprobar que esta devuelve true
void Window::setRequiredOpenGLVersion(uint major, uint minor) {
	_openGLReqMaj = major;
	_openGLReqMin = minor;
}

// Pide el perfil de compatibilidad de OpenGL (**no permitido para nuestra
// asignatura**)
void Window::setOpenGLCompatibilityFlag() { _openGLCompatibility = true; }

bool Window::checkOpenGLVersion(uint maj, uint min) {
	return maj > _openGLReqMaj || (maj == _openGLReqMaj && min >= _openGLReqMin);
}


bool Window::createWindow(const std::string &title, uint flags, uint posx,
	uint posy, uint width, uint height) {
	if (window)
		ERRT("The window is already created. Destroy it if you don't need it any "
			"more");

	_title = title;

	try {
		window = WindowHW::createWindow(this, title, flags, posx, posy, width, height,
			_openGLReqMaj, _openGLReqMin, _openGLCompatibility);
	}
	catch (std::runtime_error &) {
		// Ha ocurrido un error creando la ventana física. La causa del error está en el log
		return false;
	}

	/* Comprobar que hemos obtenido lo que pedíamos... */
	int maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	if (glGetError() != GL_NO_ERROR || !checkOpenGLVersion(maj, min)) {
		window.reset();
		return false;
	}

	_openGLVersion = GLVersion(maj, min);

	std::string glVersion = "GL_VERSION_" + std::to_string(maj) + "_" + std::to_string(min);

	INFO("Ventana creada. Soporte de OpenGL: " + glVersion);

	/* Comprobar la presencia de GLEW */
	if (!_glewReady) {
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			window.reset();
			ERRT("Unable to initialize GLEW");
		}


		// Resetear el error documentado en
		// http://www.opengl.org/wiki/OpenGL_Loading_Library
		glGetError();

		CHECK_GL();

		/* Check that glew supports this version */
		if (!glewIsSupported(glVersion.c_str())) {
			window.reset();
			ERR("Unable to initialize GLEW"); // Strangely, AMD R7 200 fails to initialize GL 4.5 when it should...
			return false;
		}
		_glewReady = true;
	}

	if (window->hasDebugContext())
		installDebugContextCallback();

	GLint fbo;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo);
	if (fbo)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_FRONT_LEFT,
		GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,
		&_redBits);
	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_FRONT_LEFT,
		GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
		&_greenBits);
	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_FRONT_LEFT,
		GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,
		&_blueBits);
	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_FRONT_LEFT,
		GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
		&_alphaBits);
	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
		GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
		&_depthBits);
	if (glGetError() != GL_NO_ERROR) {
		_depthBits = 0;
	}
	glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
		GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
		&_stencilBits);
	if (glGetError() != GL_NO_ERROR) {
		_stencilBits = 0;
	}
	if (fbo)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	uint w, h;

	window->getSize(w, h);
	reshaped(w, h);
	CHECK_GL();

	registerEventHandlers();

	return true;
}

void Window::registerEventHandlers() {
	App &instance = App::getInstance();


	keyboardSubsId = instance.keyboardEvents.addListener(std::bind(&Window::onKeyboardEvent, this, std::placeholders::_1));
	mouseMotionSubsId = instance.mouseMotionEvents.addListener(std::bind(&Window::onMouseMotionEvent, this, std::placeholders::_1));
	mouseButtonSubsId = instance.mouseButtonEvents.addListener(std::bind(&Window::onMouseButtonEvent, this, std::placeholders::_1));
	mouseWheelSubsId = instance.mouseWheelEvents.addListener(std::bind(&Window::onMouseWheelEvent, this, std::placeholders::_1));

}

void Window::deregisterEventHandlers() {
	App &instance = App::getInstance();

	instance.keyboardEvents.removeListener(keyboardSubsId);
	instance.mouseMotionEvents.removeListener(mouseMotionSubsId);
	instance.mouseButtonEvents.removeListener(mouseButtonSubsId);
	instance.mouseWheelEvents.removeListener(mouseWheelSubsId);
}

void Window::setFullScreen(bool fs) {
	window->setFullScreen(fs);
}

void Window::setRenderer(std::shared_ptr<Renderer> r) {
	addRenderer(0, r);
}

void Window::addRenderer(int order, std::shared_ptr<Renderer> r) {
	assert(r);
	//if (renderers.find(order) != renderers.end())
	//  ERRT("Ya hay un renderer con esa prioridad");

	renderers[order] = r;
	r->dispatchSetup();
	resizeRenderer(r);
}

std::shared_ptr<Renderer> Window::getRenderer(int order) {
	auto r = renderers.find(order);
	if (r == renderers.end()) {
		ERRT("Ese renderer no existe");
	}

	return r->second;
}

void Window::removeRenderer(int order) {
	renderers.erase(order);
}


void Window::draw() {
	assert(window);

	glstats.beginFrame();

	for (auto r : renderers) {
		r.second->preRender();
		r.second->render();
		r.second->postRender();
	}

	glstats.endFrame();

	if (!renderers.empty() && _showBuffer != Window::COLOR_BUFFER) {
		_bufferRenderer->showBuffer();
	}

	if (_showGUI || _showfps || _showConsole) {
		drawGUIandStats();
	}

	if (_showHelp)
		_helpOverlay->render();
}

void Window::drawGUIandStats() {
	GLStateCapturer<PolygonModeState> prevPolygonMode;
	ColorMasksState colorMask;
	StencilTestEnabledState stencilEnabled;
	GLStateCapturer<FrameBufferObjectState<GL_DRAW_FRAMEBUFFER>> prevFrameBuffer;
	GLStateCapturer<ViewportState> prevViewportSettings;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glViewport(0, 0, _width, _height);

	GLint prevDrawBuffer = -1;
	if (window->hasStereo()) {
		glGetIntegerv(GL_DRAW_BUFFER, &prevDrawBuffer);
		glDrawBuffer(GL_BACK);
	}

	bool needRenderGUI = false;
	if (_showGUI) {
		for (auto &r : renderers) {
			for (size_t i = 0; i < r.second->getNumPanels(); ++i)
			{
				if (!needRenderGUI) {
					getWindowHW()->initGUIRender();
					needRenderGUI = true;
				}
				r.second->getPanel(i)->render();
			}
		}
	}

	if (_showConsole) {
		if (!needRenderGUI) {
			getWindowHW()->initGUIRender();
			needRenderGUI = true;
		}
		console->render();
	}

	if (_showfps) {
		if (!statspanel)
			buildStatsPanel();
		if (!needRenderGUI) {
			getWindowHW()->initGUIRender();
			needRenderGUI = true;
		}
		statspanel->render();
	}


	if (needRenderGUI) {
		GUILib::finishFrame();
		GUILib::renderFrame();
	}

	if (prevDrawBuffer != -1) {
		glDrawBuffer(prevDrawBuffer);
	}

	prevFrameBuffer.restore();
	prevViewportSettings.restore();
	stencilEnabled.restore();
	colorMask.restore();
	prevPolygonMode.restore();
}

void Window::swapBuffers() {
	window->swapBuffers();
}

void Window::resizeRenderer(std::shared_ptr<Renderer> r) {
	std::shared_ptr<CameraHandler> c = r->getCameraHandler();
	if (c)
		c->resized(_width, _height);
	r->reshape(_width, _height);
}

void Window::reshaped(int w, int h) {
	_width = w;
	_height = h;
	for (auto r : renderers) {
		resizeRenderer(r.second);
	}
}

void Window::buildStatsPanel() {
	statspanel = std::shared_ptr<Panel>(new Panel("Stats"));
	fpsWidget = std::make_shared<LineChartWidget>("FPS", 100, 80, 1);
	msPerFrameWidget = std::make_shared<LineChartWidget>("ms/frame", 100, 80, 1);
	samplesPassedWidget = std::make_shared<LineChartWidget>("samples", 100, 80, 1);
	primitivesGeneratedWidget = std::make_shared<LineChartWidget>("primitives", 100, 80, 1);
	auto extendedStatsCB = std::make_shared<CheckBoxWidget>("Collect extended stats");
	extendedStatsCB->getValue().addListener([&](bool set) {
		glstats.collectExtendedStats(set);
		verticesSubmittedWidget->setVisible(set);
		primitivesSubmittedWidget->setVisible(set);
		vertexShaderInvWidget->setVisible(set);
		tessControlShaderInvWidget->setVisible(set);
		tessEvalShaderInvWidget->setVisible(set);

		fragmentShaderInvWidget->setVisible(set);
		computeShaderInvWidget->setVisible(set);
		clippingInWidget->setVisible(set);
		clippingOutWidget->setVisible(set);
	});
	verticesSubmittedWidget = std::make_shared<LineChartWidget>("vertices submitted", 100, 80, 1);
	verticesSubmittedWidget->setVisible(false);
	primitivesSubmittedWidget = std::make_shared<LineChartWidget>("primitives submitted", 100, 80, 1);
	primitivesSubmittedWidget->setVisible(false);
	fragmentShaderInvWidget = std::make_shared<LineChartWidget>("fragment shader execs", 100, 80, 1);
	fragmentShaderInvWidget->setVisible(false);
	vertexShaderInvWidget = std::make_shared<Label>("");
	vertexShaderInvWidget->setVisible(false);
	tessControlShaderInvWidget = std::make_shared<Label>("");
	tessControlShaderInvWidget->setVisible(false);
	tessEvalShaderInvWidget = std::make_shared<Label>("");
	tessEvalShaderInvWidget->setVisible(false);
	computeShaderInvWidget = std::make_shared<Label>("");
	computeShaderInvWidget->setVisible(false);

	clippingInWidget = std::make_shared<LineChartWidget>("clipping input prim.", 100, 80, 1);
	clippingInWidget->setVisible(false);
	clippingOutWidget = std::make_shared<LineChartWidget>("clipping output prim.", 100, 80, 1);
	clippingOutWidget->setVisible(false);

	statspanel->addWidget(fpsWidget);
	statspanel->addWidget(msPerFrameWidget);
	statspanel->addWidget(samplesPassedWidget);
	statspanel->addWidget(primitivesGeneratedWidget);
	statspanel->addWidget(extendedStatsCB);
	statspanel->addWidget(verticesSubmittedWidget);
	statspanel->addWidget(primitivesSubmittedWidget);
	statspanel->addWidget(vertexShaderInvWidget);
	statspanel->addWidget(tessControlShaderInvWidget);
	statspanel->addWidget(tessEvalShaderInvWidget);
	statspanel->addWidget(computeShaderInvWidget);
	statspanel->addWidget(clippingInWidget);
	statspanel->addWidget(clippingOutWidget);
}


void Window::update(uint ms) {
	static uint elapsed = 0, nframes = 0;
	static uint64_t samplesPassedAccum = 0, primitivesGeneratedAccum = 0, verticesSubmittedAccum = 0, primitivesSubmittedAccum = 0, fragmentShaderInvAccum = 0;
	static uint64_t clippingInAccum = 0, clippingOutAccum = 0;
	static float renderElapsed = 0.0f;

	elapsed += ms;
	nframes++;
	renderElapsed += glstats.getFrameDuration();
	samplesPassedAccum += glstats.getValue(GLStats::Query::SamplesPassed);
	primitivesGeneratedAccum += glstats.getValue(GLStats::Query::PrimitivesGenerated);
	verticesSubmittedAccum += glstats.getValue(GLStats::Query::VerticesSubmittedExt);
	primitivesSubmittedAccum += glstats.getValue(GLStats::Query::PrimitivesSubmittedExt);
	fragmentShaderInvAccum += glstats.getValue(GLStats::Query::FragmentShaderInvocationsExt);
	clippingInAccum += glstats.getValue(GLStats::Query::ClippingInputPrimitivesExt);
	clippingOutAccum += glstats.getValue(GLStats::Query::ClippingOutputPrimitivesExt);

	if (elapsed >= 100) {
		float fps = nframes * elapsed / 10.0f;

		if (fpsWidget) {
			fpsWidget->pushValue(fps);
			msPerFrameWidget->pushValue(renderElapsed / nframes);
			samplesPassedWidget->pushValue(static_cast<float>(samplesPassedAccum) / nframes);
			primitivesGeneratedWidget->pushValue(static_cast<float>(primitivesGeneratedAccum) / nframes);
			verticesSubmittedWidget->pushValue(static_cast<float>(verticesSubmittedAccum) / nframes);
			primitivesSubmittedWidget->pushValue(static_cast<float>(primitivesSubmittedAccum) / nframes);
			vertexShaderInvWidget->setText("Vertex shader execs: " + std::to_string(glstats.getValue(GLStats::Query::VertexShaderInvocationsExt)));
			tessControlShaderInvWidget->setText("Tess. control shader execs: " + std::to_string(glstats.getValue(GLStats::Query::TessControlShaderPatchesExt)));
			tessEvalShaderInvWidget->setText("Tess. evaluation shader execs: " + std::to_string(glstats.getValue(GLStats::Query::TessEvalShaderInvocationsExt)));
			fragmentShaderInvWidget->pushValue(static_cast<float>(fragmentShaderInvAccum) / nframes);
			computeShaderInvWidget->setText("Compute shader execs: " + std::to_string(glstats.getValue(GLStats::Query::ComputeShaderInvocationsExt)));
			clippingInWidget->pushValue(static_cast<float>(clippingInAccum) / nframes);
			clippingOutWidget->pushValue(static_cast<float>(clippingOutAccum) / nframes);

		}
		elapsed = 0;
		nframes = 0;
		samplesPassedAccum = primitivesGeneratedAccum = verticesSubmittedAccum = primitivesSubmittedAccum = fragmentShaderInvAccum = 0;
		clippingInAccum = clippingOutAccum = 0;
		renderElapsed = 0.0f;
	}

	for (auto r : renderers) {
		r.second->update_camera(ms);
		r.second->update(ms);
	}
}

void Window::setTitle(const std::string &title) {
	assert(window);
	window->setTitle(title);
}

void Window::setMousePosition(uint x, uint y) {
	assert(window);
	window->setMousePosition(x, y);
}

void Window::showMouseCursor(bool show) {
	assert(window);
	window->showMouseCursor(show);
}

bool Window::saveColorBuffer(const std::string &filename, GLint framebuffer) {
	PGUPV::Image image(_width, _height, 24);
	GLint oldRead;
	glGetIntegerv(GL_READ_BUFFER, &oldRead);
	glReadBuffer(framebuffer);

  GLStateCapturer<PixelPackState> packState;

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, image.getPixels());
	glReadBuffer(oldRead);
	image.save(filename);
	INFO("Imagen guardada a " + filename);
	return true;
}

uint Window::getStencilSize() {
	assert(window != nullptr);
	int bpp;
	CHECK_GL();
	glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_STENCIL,
		GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
		&bpp);
	CHECK_GL2(
		"Error pidiendo a OpenGL el número de bits de los elementos del stencil");
	return bpp;
}

void Window::showBuffer(ShowBuffer buffer) {
	if (buffer == STENCIL_BUFFER && !hasStencilBuffer()) {
		WARN("No se puede mostrar el buffer de stencil porque la ventana no se "
			"creó con uno");
		return;
	}
	if (buffer == DEPTH_BUFFER && !hasDepthBuffer()) {
		WARN(
			"No se puede mostrar el z-buffer porque la ventana no se creó con uno");
		return;
	}

	if (buffer != COLOR_BUFFER && _bufferRenderer == nullptr) {
		_bufferRenderer = std::make_unique<BufferRenderer>(*this);
	}
	_showBuffer = buffer;
}

void Window::showGUI(bool show) { _showGUI = show; }

bool Window::isGUIVisible() const { return _showGUI; }

GLVersion Window::getOpenGLVersion(void) const { return _openGLVersion; }

#ifdef _WIN32
#undef GLAPIENTRY
#define GLAPIENTRY __stdcall
#endif
void GLAPIENTRY debugCallbackFunc(GLenum source, GLenum type, GLuint /*id*/,
	GLenum severity, GLsizei /*length*/, const GLchar *message, const void * /*userParam*/) {
	std::ostringstream os;

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		os << "(!!!) ";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		os << "(!! ) ";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		os << "(!_) ";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		os << "(___) ";
		break;
	};

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		// Message from OpenGL
		os << "GL Debug: ";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		os << "Window System Debug: ";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		os << "GLSL Compiler Debug: ";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		os << "Third Party Debug: ";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		os << "App Debug: ";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		os << "Other Debug: ";
		break;
	};

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		os << "(ERROR) ";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		os << "(DEPRECATED) ";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		os << "(UNDEF BEHAV) ";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		os << "(PERFORMANCE) ";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		os << "(PORTABILITY) ";
		break;
	case GL_DEBUG_TYPE_MARKER:
		os << "(MARKER) ";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		os << "(PUSH) ";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		os << "(POP) ";
		break;
	case GL_DEBUG_TYPE_OTHER:
		os << "(OTHER) ";
		break;
	};
	os << message;

	// In case of HIGH severities, I will issue an error, otherwise a warning
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		ERR(os.str());
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		FRAME(os.str());
	else
		WARN(os.str());
}

void Window::installDebugContextCallback() {
	if (!getOpenGLVersion().isGreaterEqual(GLVersion(4, 3))) {
		WARN(
			"La versión en ejecución de OpenGL no soporta contextos de depuración");
	}
	else {
		glDebugMessageCallback(debugCallbackFunc, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
}

void Window::setGLDebugContextLevel(int minLevel) {
	if (!getOpenGLVersion().isGreaterEqual(GLVersion(4, 3))) {
		WARN(
			"La versión en ejecución de OpenGL no soporta contextos de depuración");
	}
	else {
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
			GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr,
			minLevel <= 0 ? GL_TRUE : GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0,
			nullptr, minLevel <= 1 ? GL_TRUE : GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM,
			0, nullptr, minLevel <= 2 ? GL_TRUE : GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0,
			nullptr, minLevel <= 3 ? GL_TRUE : GL_FALSE);
	}
}

void Window::onKeyboardEvent(const PGUPV::KeyboardEvent &e) {
	for (auto r : renderers) {
		if (r.second->keyboard(e))
			return;
		std::shared_ptr<CameraHandler> c = r.second->getCameraHandler();
		if (c && c->keyboard(e))
			return;
	}
}

static bool forwardEvent(const PGUPV::MouseMotionEvent &m, std::shared_ptr<Renderer> renderer) {
	if (renderer->mouse_move(m))
		return true;
	std::shared_ptr<CameraHandler> c = renderer->getCameraHandler();
	if (c && c->mouse_move(m))
		return true;
	return false;
}

void Window::onMouseMotionEvent(const PGUPV::MouseMotionEvent &m) {
	lastXMousePos = m.x;
	lastYMousePos = m.y;

	auto active = activeRenderer.lock();
	if (active) {
		forwardEvent(m, active);
	}
	else {
		for (auto r = renderers.rbegin(); r != renderers.rend(); ++r) {
			auto renderer = r->second;
			if (forwardEvent(m, renderer))
				return;
		}
	}
}

static bool forwardEvent(const PGUPV::MouseButtonEvent &m, std::shared_ptr<Renderer> r, uint windowheight) {
	if (!r->isInViewport(m.x, windowheight - m.y))
		return false;
	if (r->mouse_button(m))
		return true;
	std::shared_ptr<CameraHandler> c = r->getCameraHandler();
	if (c && c->mouse_button(m))
		return true;
	return false;
}

void Window::onMouseButtonEvent(const PGUPV::MouseButtonEvent &m) {
	lastXMousePos = m.x;
	lastYMousePos = m.y;

	// Button down sets the current window to the viewport where the pointer is located, 
	// and also wants the events
	if (m.state == ButtonState::Pressed) {
		for (auto r = renderers.rbegin(); r != renderers.rend(); ++r) {
			auto renderer = r->second;
			if (forwardEvent(m, renderer, _height)) {
				activeRenderer = renderer;
				return;
			}
		}
	}
	else {
		// Button released
		std::shared_ptr<Renderer> r = activeRenderer.lock();
		if (r) {
			forwardEvent(m, r, _height);
			activeRenderer.reset();
		}
		else {
			ERR("Released button with no active renderer. Ignoring...");
		}
	}
}

void Window::onMouseWheelEvent(const PGUPV::MouseWheelEvent &m) {
	for (auto r = renderers.rbegin(); r != renderers.rend(); ++r) {
		auto renderer = r->second;
		if (!renderer->isInViewport(lastXMousePos, _height - lastYMousePos)) continue;
		if (renderer->mouse_wheel(m))
			return;
		std::shared_ptr<CameraHandler> c = renderer->getCameraHandler();
		if (c && c->mouse_wheel(m))
			return;
	}
}

void Window::onJoyAxisMotionEvent(const PGUPV::JoystickMotionEvent &j) {
	for (auto r : renderers) {
		if (r.second->joystick(j))
			return;
		std::shared_ptr<CameraHandler> c = r.second->getCameraHandler();
		if (c && c->joystick(j))
			return;
	}
}

void Window::onJoyHatMotionEvent(const PGUPV::JoystickHatMotionEvent &j) {
	for (auto r : renderers) {
		if (r.second->joystick_hat(j))
			return;
		std::shared_ptr<CameraHandler> c = r.second->getCameraHandler();
		if (c && c->joystick_hat(j))
			return;
	}
}

void Window::onJoyButtonEvent(const PGUPV::JoystickButtonEvent &j) {
	for (auto r : renderers) {
		if (r.second->joystick_button(j))
			return;
		std::shared_ptr<CameraHandler> c = r.second->getCameraHandler();
		if (c && c->joystick_button(j))
			return;
	}
}


bool Window::isFullScreenSet() const {
	return window->isFullScreen();
};

bool Window::hasDoubleBuffer() const {
	return window->hasDoubleBuffer();
};

void Window::showHelp(bool show) {
	_showHelp = show;
	if (_showHelp) {
		if (!_helpOverlay) {
			_helpOverlay = std::make_shared<TextOverlay>(App::getShortcutsHelp());
			_helpOverlay->reshape(width(), height());
		}
		addRenderer(1000, _helpOverlay);
	}
	else
		removeRenderer(1000);
}

void Window::maximize() {
	window->maximize();
}

void Window::minimize() {
	window->minimize();
}

void Window::restore() {
	window->restore();
}

