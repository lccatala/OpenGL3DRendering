#ifndef _APP_H
#define _APP_H

#include <functional>
#include <vector>
#include <GL/glew.h>        // for GLint
#include <cstdint>
#include <map>              // for map
#include <memory>           // for unique_ptr, shared_ptr
#include <stddef.h>         // for size_t
#include <string>          // for string

#include "common.h"
#include "intervals.h"
#include "shaderLibrary.h"
#include "properties.h"
#include "events.h"         // for KeyCode, JoystickAxisMotionEventsSource, JoystickButtonEventsSource, JoystickHatMotionEventsSource, KeyboardEventsSource, JoystickButtonEvent (ptr only), JoystickHatMotionEvent (ptr only), JoystickMotionEvent (ptr only), KeyboardEvent (ptr only), MouseButtonEventsSource, MouseMotionEventsSource, MouseWheelEventsSource


namespace PGUPV {
  /**
  \class App
  Clase que se encarga de inicializar la aplicación. Es una clase Singleton (es
  decir, no se pueden instanciar nuevos objetos de la misma, sólo se puede acceder
  a una única instancia con la función getInstance). Normalmente obtendrás una
  referencia al objeto App en la función main de tu programa, y la usarás para
  configurar la ventana OpenGL.
  Las funciones principales, por orden de uso son:

  -# App & App#getInstance(): devuelve una referencia al (único) objeto App
  -# bool App#initApp(int argc, char **argv, uint flags): se le pasa la línea de
  comandos recibida por la función main, varios flags que indican las
  características que
  debe tener la ventana OpenGL. Las constantes que se pueden usar están
  definidas en el fichero
  window.h, y se pueden combinar varias con el operador OR bit a bit (|).
  -# Window &App#getWindow(): devuelve una referencia a la ventana, para, por
  ejemplo, cambiar
  el objeto encargado de dibujar la escena.
  -# int App#run(void): función que ejecuta el bucle del visualizador. Se encarga
  de atender a los
  eventos y gestionar las callbacks. El flujo de ejecución no saldrá de
  esta función
  hasta que se llame a la función App.done(true).

  Ejemplo de uso típico:
  \code{.cpp}
  int main(int argc, char *argv[]) {
  App *myApp = App::getInstance();
  myApp->initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
  PGUPV::MULTISAMPLE);
  return myApp->run();
  }
  \endcode
  */

  class CommandLineProcessor;
  class EventProcessor;
  class EventSource;
  class Gamepad;
  class HW;
  class Keyboard;
  class StatsClass;
  class Window;
  
  
  /**
  Tipos de diálogo
  */
  enum class DialogType { DLG_ERROR, DLG_WARNING, DLG_INFORMATION };



  class App {
  public:
    /**
    \returns Devuelve una referencia al objeto App. No puedes crear nuevos objetos
    App, sólo puedes pedir una referencia al único que existe
    */
    static App &getInstance(void);
    ~App();
    /**
    Llama a esta función para poner en marcha la aplicación. Esta función sólo
    termina al invocar a la función done(), y se encarga de gestionar los eventos de la
    aplicación.
    \returns un código de error
    */
    int run(void);
    void done(int errorCode = 0) {
      _errorCode = errorCode;
      _appDone = true;
    };
    bool isDone() const { return _appDone; }
    void pause(bool pause = true) { _paused = pause; };
    bool isPaused(void) const { return _paused; };
    bool initApp(int argc, char **argv, uint flags);
    bool initApp(PGUPV::CommandLineProcessor cmdLine, uint flags);
    void showFPS(bool show = true) { _show_fps = show; };
    bool isFPSShowing() const { return _show_fps; };
    Window &getWindow(uint index = 0) const;
    /**
    Devuelve el tiempo transcurrido desde el inicio de la aplicación,
    en segundos. No tiene en cuenta los periodos de pausa (es decir,
    cuando la pausa está activada, el tiempo no corre).
    \returns segundos desde el inicio de la aplicación
    */
    double getAppTime();
    /**
    \return El número de milisegundos transcurridos desde el
    inicio de la aplicación
    */
    uint getCurrentMillis();

    /**
     Función similar a App::getCurrentMillis(), pero usa un reloj de más resolución.
     \return El número de microsegundos transcurridos desde la epoch.
     */
    static int64_t getCurrentMicroSecs();
    /**
    Establece el tamaño inicial de la ventana.
    \param width Ancho de la ventana, en píxeles
    \param height Alto de la ventana, en píxeles
    */
    void setInitWindowSize(int width, int height);
    /**
    Establece la posición inicial de la ventana.
    \param x Coordenada x de la esquina superior izquierda, en píxeles
    \param y Coordenada y de la esquina superior izquierda, en píxeles
    */
    void setInitWindowPos(int x, int y);
    /**
    Establece la versión mínima de OpenGL que necesita esta aplicación para
    funcionar.
    El driver puede devolver una versión superior. Por defecto se pide como mínimo
    OpenGL 4.0
    \param minor versión secundaria de OpenGL (p.e., 2 para pedir 4.2)
    */
    void setMinimumGLVersion(uint minor);

    /**
    Establece la versión preferida de OpenGL que necesita esta aplicación para
    funcionar. Si esta versión no está disponible, se buscarán versiones menores
    hasta la indicada por App#setMinimumGLVersion
    El driver puede devolver una versión superior.
    \param major versión principal de OpenGL (p.e., 4)
    \param minor versión secundaria de OpenGL (p.e., 2 para pedir 4.2)
    */
    void setPrefGLVersion(uint major, uint minor);

    void setFramesToLive(long frames);
    void captureSnapshots(PGUPV::Intervals ints);
    // Devuelve el frame actual
    ulong getCurrentFrame() { return _current_frame; };

    /**
      Recibe una función que se invocará justo antes de terminar la aplicación
      (por ejemplo, para parar una librería o para vaciar una cache).
      El orden de ejecución de las funciones será el inverso al del orden en el
      que se pasaron a este método.
      \param f la función a invocar justo antes de salir de la aplicación
      */

    void onShutdown(std::function<void()> f);

    // Devuelve la unidad de textura utilizada internamente por la librería (es la
    // última unidad de textura disponible en el sistema. Esta unidad está reservada
    // y no la deberías utilizar)
    // \return Número de unidad (desde 0 a GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS-1)
    static GLint getScratchUnitTextureNumber();
    /**
    \return El número de gamepads encontrados en el sistema
    */
    size_t getNumGamepads();

    /**
    It will take a snapshot when the frame is done
    */
    void snapshot() {
      _take_snapshot = true;
    }

    /**
    \return true si la tecla se ha pulsado en este frame
    */
    static bool isKeyDown(PGUPV::KeyCode k);

    /**
    \return true si la tecla está pulsada
    */
    static bool isKeyPressed(PGUPV::KeyCode k);

    /**
    \return true si la tecla se ha soltado en este frame
    */
    static bool isKeyUp(PGUPV::KeyCode k);

    /**
    Establece un objeto receptor de las estadísticas
    */

    static void setStatsObj(std::shared_ptr<StatsClass> statsObject);

    /**
    Establece el procesador de eventos
    */
    static void setEventProcessor(std::unique_ptr<EventProcessor> eventProcessor);

    /**
     \return el objeto receptor de estadísticas
     */
    static std::shared_ptr<StatsClass> getStatsObj();

    /**
    Muestra un cuadro de diálogo con un mensaje
    \param type Tipo del diálogo (App::DialogType::{ERROR, WARNING, INFORMATION})
    \param title Título del diálogo
    \param message Mensaje del diálogo
    \return un entero que indica el botón que ha pulsado el usuario: 100 si quiere mostrar el Log o
      1 en otro caso
    */
    int showMessageDialog(DialogType type, const std::string &title, const std::string &message);

    /**
    \return Una cadena con las versiones de todas las librerías que se están usando
    */
    std::string collectLibVersions();

    /**
    Devuelve el valor de la variable de estado indicada por name. Estas variables se almacenan en fichero,
    por lo que se mantienen de una ejecución a otra
    \param name nombre de la propiedad. Para propiedades predefinidas usa las constantes definidas por App (empiezan por PROP_)
    \param value el valor de la propiedad, si existe
    \return true si la variable existe, false en otro caso
    */
	template <typename T>
	bool getProperty(const std::string &name, T &value) {
		return properties.value(name, value);
	}

    /**
    Establece el valor de la propiedad indicada, que se almacenará en un fichero al terminar la aplicación.
    \param name nombre la propiedad. Para propiedades predefinidas usa las constantes definidas por App (empiezan por PROP_)
    \param value valor a almacenar
    */
    void setProperty(const std::string &name, const std::string &value);

    /**
    Elimina la entrada de la lista de propiedades
    \param name nombre de la propiedad
    \return si la propiedad existía y se borró
    */
    bool removeProperty(const std::string &name);

    //! Versión más alta encontrada que funciona (por ejemplo, en un sistema que soporta GL 4.5, será 4.5)
    static const char *PROP_HIGHEST_GL_VERSION_SEEN;
    //! Ruta completa del comando ffmpeg
    static const char *PROP_FFMPEG_EXEC_PATH;
    /**
    Si quieres recibir eventos de teclado suscríbete a este objeto así:

    App::getInstance().keyboardEvents.addListener(std::bind(&TuClase::func, instancia, std::placeholders::_1));

    donde, por ejemplo tienes:

    void TuClase::func(const KeyBoardEvent &k) {
    // hacer algo con k
    }

    e instancia es el objeto cuya clase 'func' quieres invocar
    */


    KeyboardEventsSource keyboardEvents;
    MouseMotionEventsSource mouseMotionEvents;
    MouseButtonEventsSource mouseButtonEvents;
    MouseWheelEventsSource mouseWheelEvents;
    JoystickAxisMotionEventsSource joystickAxisMotionEvent;
    JoystickHatMotionEventsSource joystickHatMotionEvent;
    JoystickButtonEventsSource joystickButtonEvent;

    /**
    \return Una referencia a la biblioteca de programas
    */
    static ShaderLibrary &getShaderLibrary();

    /**
    \return Ayuda sobre las teclas disponibles durante la ejecución
    */
    static std::string getShortcutsHelp();

    void setEventSource(std::unique_ptr<EventSource> evSource);
    EventSource &getEventSource() const { return *eventSource.get(); };

    /**
    \return El índice de la ventana indicada (es el orden en que se creó, empezando en 0)
      -1 Si no la encuentra
    */
    int getWindowIndex(const Window *w) const;

    /**
    \return Un puntero a la ventana indicada
    */
    Window *getWindowPtr(const uint index) const { return m_windows[index]; }
    /**
     Añade una callback que se ejecutará antes de dibujar el siguiente frame
     \param callback Una función que recibe una referencia a la ventana que se está dibujando
     \return el identificador de la callback (se puede usar para borrarla después)
     */
    size_t addPreRender(std::function<void()> callback);
    /**
     Quita la callback indicada
     \param id identificador de la callback a eliminar (devuelto por App::addPreRender)
     */
    void removePreRender(int id);

    /**
     Añade una callback que se ejecutará después de dibujar el frame actual
     \param callback Una función que recibe la referencia a la ventana que se acaba de dibujar
     \return el identificador de la callback (se puede usar para borrarla después)
     */
    size_t addPostRender(std::function<void()> callback);
    /**
     Quita la callback indicada
     \param id identificador de la callback a eliminar (devuelto por App::addPreRender)
     */
    void removePostRender(size_t id);

    /**
    Ignora las cámaras conectadas al sistema
    \param ignore true para actuar como si no hubieran cámaras conectadas
    */
    void setIgnoreCameras(bool ignore) {
      ignoreCameras = ignore;
    }

    /**
    Devuelve si se están ignorando las cámaras del sistema
    */
    bool isIgnoreCamerasSet() {
      return ignoreCameras;
    }

    /**
    No almacena ni carga el estado del GUI (posición y tamaño de los paneles)
    \param ignore true para ignorar el estado del GUI
    */
    void setForgetGUIState(bool forget) {
      ignoreGUIState = forget;
    }

    /**
    Devuelve si se están almacenando el estado del GUI
    */
    bool isForgetGUIState() {
      return ignoreGUIState;
    }

	// Millisecs since last frame
	static unsigned long getDeltaTime() {
		return _elapsed;
	}
    
  private:
    void processEvents();
    void update(uint elapsedMs);
    void render();
    App();
    void takeSnapshot();
    void initJoysticks();
    int _errorCode;
    bool _appDone, _paused, _show_fps, _take_snapshot, _destroyed;
	static unsigned int _elapsed;
    ulong _current_frame;
    std::vector<Window *> m_windows;
    std::vector<std::unique_ptr<Gamepad>> gameControllers;
    double _running_time;
    uint initX, initY, initWidth, initHeight;
    int64_t ftl;
    Intervals snapshots;
    int preferredMajorGLVer, preferredMinorGLVer, minimumGLVer;
    void destroy(void);
    std::vector<std::function<void()>> onShutdownFunctions;

    std::shared_ptr<StatsClass> stats;
    std::unique_ptr<HW> hw;

    std::unique_ptr<Keyboard> keyboardCache;

    friend class AppEventProcessor;

    void onKeyboardEvent(const KeyboardEvent &);
    void onJoyAxisMotionEvent(const PGUPV::JoystickMotionEvent &e);
    void onJoyHatMotionEvent(const PGUPV::JoystickHatMotionEvent &e);
    void onJoyButtonEvent(const PGUPV::JoystickButtonEvent &e);

    void loadAppStatus(int idx);
    void saveAppStatus(int idx);

    KeyboardEventsSource::SubscriptionId keyboardSubsId;
    JoystickAxisMotionEventsSource::SubscriptionId joyMotionSubsId;
    JoystickHatMotionEventsSource::SubscriptionId joyHatSubsId;
    JoystickButtonEventsSource::SubscriptionId joyButtonSubsId;

    ShaderLibrary shaderLib;
    Properties properties;

    std::unique_ptr<EventSource> eventSource;
    std::unique_ptr<EventProcessor> eventProcessor;
    bool ignoreCameras, ignoreGUIState;
    std::map<size_t, std::function<void()>> preRenderCallbacks, postRenderCallbacks;
  };

};

#endif
