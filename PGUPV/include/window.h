#ifndef _WINDOW_H
#define _WINDOW_H

#include <string>
#include <memory>
#include <iosfwd>     // for ostream
#include <map>
#include <GL/glew.h>
#include "glStats.h"
#include "events.h"
#include "common.h"   // for uint
#include "glVersion.h"
#include "observable.h"

namespace PGUPV {

	/**
	\class Window
	La clase Window representa una ventana del sistema operativo. Se encarga de
	crear la ventana con las características indicadas por el usuario.

	*/

	enum Flags {
		FULLSCREEN = 1,
		DOUBLE_BUFFER = 2,
		DEPTH_BUFFER = 4, /* ACCUM_BUFFER was here */
		STENCIL_BUFFER = 16,
		MULTISAMPLE = 32,
		STEREO = 64,
		RGBA = 128,
		DEBUG = 256 	// We always request a debug context when compiling in Debug. When compiling in
		// Release, you have to explicitly request it
	};

	class WindowHW;
	class Renderer;
	class BufferRenderer;
	class TextOverlay;
	class Panel;
	class LineChartWidget;
	class Label;
	class LogConsole;

	class Window {
	public:
		Window();
		~Window();
		/**
			Crea una ventana con el título y características indicadas.
			\param title: título de la ventana
			\param flags: indican las características de la ventana, como doble buffer o
			z-buffer. Ver las posibles opciones al principio del fichero window.h
			\param  posx, posy: posición de la esquina superior izquierda de la ventana
			con respecto a la pantalla.
			\param width, height: tamaño en píxeles de la zona de dibujo.
			*/
		bool createWindow(const std::string &title = "My Window",
			uint flags = PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER, uint posx = 50,
			uint posy = 50, uint width = 512, uint height = 512);
		// Muestra la ventana a pantalla completa o como una ventana
		void setFullScreen(bool fs);
		// Devuelve si la ventana está a pantalla completa o no
		bool isFullScreenSet() const;
		/**
		Establece el objeto encargado de dibujar la ventana.
		Es equivalente a llamar a addRenderer(0, r)
		*/
		void setRenderer(std::shared_ptr<Renderer> r);
		/**
		Devuelve  el objeto encargado de dibujar la ventana
		*/
		std::shared_ptr<Renderer> getRenderer(int order = 0);

		/**
		Añade un renderer a la ventana. Los renderer se aplicarán secuencialmente, según
		el orden indicado. El renderer principal es el cero. Los renderer con número
		de orden negativo se ejecutarán antes que el principal, y los positivos después.
		No puede haber dos renderer con el mismo número de orden.

		*/
		void addRenderer(int order, std::shared_ptr<Renderer> r);
		/**
		Quita de la lista de renderers el que tiene el orden indicado.
		*/
		void removeRenderer(int order);
		// Devuelve el ancho de la zona de dibujo de la ventana
		uint width() const { return _width; };
		// Devuelve el alto de la zona de dibujo de la ventana
		uint height() const { return _height; };
		// Mostrar/ocultar los frames por segundo en la ventana de título
		void showStats(bool show = true) { _showfps = show; };
		//! \return true si se está mostrando el panel de estadísticas
		bool isStatsOpen() const { return _showfps; };
		// Establecer el título de la ventana
		void setTitle(const std::string &title);
		// Mueve el puntero del ratón a las coordenadas indicadas (cuidado!
		// genera el evento de movimiento de ratón correspondiente)
		void setMousePosition(uint x, uint y);
		// Muestra/oculta el puntero del ratón
		void showMouseCursor(bool show = true);
		/**
		Muestra/oculta ayuda en pantalla sobre los atajos de teclado
		*/
		void showHelp(bool show = true);
		/**
		\return true si la ayuda se está mostrando
		*/
		bool isHelpVisible() const { return _showHelp; }
		/**
		Muestra/oculta ayuda en pantalla la consola de logging
		*/
		void showConsole(bool show = true) { _showConsole = show; }
		/**
		\return true si la consola está abierta
		*/
		bool isConsoleVisible() { return _showConsole; }
		/** Declara la versión de OpenGL que necesita la aplicación.
		\warning Llamar a esta función *antes* de a createWindow y comprobar que esta devuelve true
		*/
		void setRequiredOpenGLVersion(uint major, uint minor);
		/**
		Pide el perfil de compatibilidad de OpenGL
		\warning ¡¡¡No permitido para nuestra asignatura!!!
		*/
		void setOpenGLCompatibilityFlag();
		/**
		Escribe información sobre la ventana en el flujo
		*/
		void printInfo(std::ostream &os);
		/**
		\return La versión de OpenGL que se está ejecutando
		*/
		GLVersion getOpenGLVersion() const;
		// Estas funciones reciben y reenvían eventos. No las llama el usuario
		void reshaped(int w, int h);

		void draw();
		void drawGUIandStats();
		void swapBuffers();
		void destroy();
		void update(uint ms);
		/** Guarda en un fichero con el nombre indicado el contenido actual de la
		 ventana */
		bool saveColorBuffer(const std::string &filename,
			GLint framebuffer = GL_FRONT);
		// Devuelve el número de bits por cada elemento del stencil del framebuffer
		// asociado a GL_READ_BUFFER
		uint getStencilSize();
		// Muestra por pantalla el contenido del buffer indicado. Valores aceptados:
		enum ShowBuffer { COLOR_BUFFER, DEPTH_BUFFER, STENCIL_BUFFER };
		void showBuffer(ShowBuffer buffer);
		// Devuelve qué buffer se está mostrando
		ShowBuffer getShownBuffer() const { return _showBuffer; }
		/**
		 Muestra u oculta la interfaz de usuario
		 */
		void showGUI(bool show = true);
		/**
		 Indica si el GUI está visible o no.
		 */
		bool isGUIVisible() const;
		/**
		\return Si la ventana tiene doble buffer
		*/
		bool hasDoubleBuffer() const;
		/**
		\return Si la ventana tiene un buffer de stencil
		*/
		bool hasStencilBuffer() const { return _stencilBits > 0; };
		/**
		\return Si la ventana tiene un Zbuffer
		*/
		bool hasDepthBuffer() const { return _depthBits > 0; };

		/**
		Establece el nivel de logging del contexto de depuración de OpenGL,
		dependiendo de su severidad.
		La severidad de los mensajes de OpenGL es: ALTA, MEDIA, BAJA, NOTIFICACION
		\param minLevel establece el nivel mínimo a mostrar: 0 todos, 1: BAJA, MEDIA,
		ALTA; 2: MEDIA, ALTA; 3: ALTA, 4: ninguno
		*/
		void setGLDebugContextLevel(int minLevel);

		/**
		Maximiza la ventana */
		void maximize();
		/**
		Minimize la ventana
		*/
		void minimize();
		/**
		Restaura el tamaño de la ventana
		*/
		void restore();

		std::shared_ptr<WindowHW> getWindowHW() const { return window; };
	private:

		std::shared_ptr<WindowHW> window;
		bool checkOpenGLVersion(uint maj, uint min);
		void installDebugContextCallback();
		std::string _title;
		uint _openGLReqMaj, _openGLReqMin;
		bool _openGLCompatibility;
		static bool _glewReady;     // Hemos inicializado glew?
		std::map<int, std::shared_ptr<Renderer>> renderers;
		bool _showfps, _showHelp, _showConsole;
		ShowBuffer _showBuffer;
		std::unique_ptr<BufferRenderer> _bufferRenderer;
		std::shared_ptr<TextOverlay> _helpOverlay;
		bool _showGUI;
		GLVersion _openGLVersion;
		GLint _redBits, _greenBits, _blueBits, _alphaBits, _depthBits, _stencilBits;
		uint _width, _height;
		std::unique_ptr<LogConsole> console;
		Observable<std::string>::SubscriptionId consoleSubs;

		// Panel de estadísticas
		std::shared_ptr<Panel> statspanel;
		std::shared_ptr<LineChartWidget> fpsWidget, msPerFrameWidget, samplesPassedWidget, primitivesGeneratedWidget, verticesSubmittedWidget;
		std::shared_ptr<LineChartWidget> primitivesSubmittedWidget, fragmentShaderInvWidget, clippingInWidget, clippingOutWidget;
		std::shared_ptr<Label> vertexShaderInvWidget, tessControlShaderInvWidget, tessEvalShaderInvWidget, computeShaderInvWidget;

		GLStats glstats;

		//// Gestores de eventos 
		void onKeyboardEvent(const PGUPV::KeyboardEvent &e);
		void onMouseMotionEvent(const PGUPV::MouseMotionEvent &m);
		void onMouseButtonEvent(const PGUPV::MouseButtonEvent &m);
		void onMouseWheelEvent(const PGUPV::MouseWheelEvent &m);
		void onJoyAxisMotionEvent(const PGUPV::JoystickMotionEvent &j);
		void onJoyHatMotionEvent(const PGUPV::JoystickHatMotionEvent &j);
		void onJoyButtonEvent(const PGUPV::JoystickButtonEvent &j);
		void registerEventHandlers();
		void deregisterEventHandlers();


		inline void resizeRenderer(std::shared_ptr<Renderer> r);

		KeyboardEventsSource::SubscriptionId keyboardSubsId;
		MouseMotionEventsSource::SubscriptionId mouseMotionSubsId;
		MouseButtonEventsSource::SubscriptionId mouseButtonSubsId;
		MouseWheelEventsSource::SubscriptionId mouseWheelSubsId;
		JoystickAxisMotionEventsSource::SubscriptionId joyMotionSubsId;
		JoystickHatMotionEventsSource::SubscriptionId joyHatSubsId;
		JoystickButtonEventsSource::SubscriptionId joyButtonSubsId;

		std::weak_ptr<Renderer> activeRenderer;
		uint lastXMousePos, lastYMousePos;

		void buildStatsPanel();
	};

}; // namespace

#endif
