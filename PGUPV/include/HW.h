#pragma once

#include <functional>
#include <memory>
#include <map>

#ifndef _WIN32
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "common.h"
#include "eventSource.h"

/**
\file HW.h

Clase abstracta que encapsula las funciones de interacción con el hardware.
Normalmente esta clase se implementará usando SDL, freeglut, glfw, etc.

*/
namespace PGUPV {

  class Window;
  enum class DialogType;
  class WindowHW;

  class HW {
  public:
    HW();
    virtual ~HW();

    /**
    Llamar a esta función para liberar los recursos de la librería
    */
    void terminate(void);
    // Devuelve el número de gamepads conectados al sistema
    size_t getNumGamepads();

    /**
    \return El número de milisegundos transcurridos desde el inicio de la aplicación
    */
    uint currentMillis();
    /**
    Pausa la ejecución el número de milisegundos indicados
    ¡Cuidado! Esta pausa detiene todo el procesado de eventos y la aplicación se queda
    congelada.
    \param ms milisegundos a suspender la ejecución
    */
    void sleep(uint ms);

    /**
    Mueve el puntero del ratón a la posición de la ventana indicada
    */
    static void moveMousePointerInWindow(int x, int y);

    // Esto se romperá eventualmente (p.e. 3.0.0), pero para esos entonces sólo habrá que quitar 
    // la guarda
#if SDL_MAJOR_VERSION>=2 && SDL_MINOR_VERSION>= 0 && SDL_PATCHLEVEL>=4
    /**
    Mueve el puntero del ratón a la posición de la pantalla indicada
    */
    static void moveMousePointerTo(int x, int y);
#endif
    /**
    Devuelve la versión de la librería subyacente, en forma de:

    \<Librería\>: version,
    p.e.: SDL: 2.0.3
    */
    std::string getLibVersionString();

    /**
    Muestra un cuadro de diálogo con un mensaje
    \param type Tipo del diálogo (HW::DialogType::{ERROR, WARNING, INFORMATION})
    \param title Título del diálogo
    \param body Mensaje del diálogo
    \return 100 si el usuario pulsa el botón de mostrar el log y terminar, 101 para terminar la aplicación,
    1 en cualquier otro caso
    */
    int showMessageBox(DialogType type, const std::string &title, const std::string &body);
  };

  class Window;
  class WindowHW {
  public:
    static std::shared_ptr<WindowHW> createWindow(Window *container, const std::string &title, uint flags, uint posx,
      uint posy, uint width, uint height, uint glMajor, uint glMinor, bool glCompatibility);
    ~WindowHW();
    
    std::string requestedGLVersion() { return reqGLVersion; }

    bool hasDoubleBuffer() const {
      return gotDoubleBuffer;
    }

    bool hasDebugContext() const {
      return gotDebugContext;
    }

    bool hasStereo() const {
      return gotStereo;
    }

    void setFullScreen(bool fs);

    void setTitle(const std::string &title);

    void swapBuffers();

    bool isFullScreen() const {
      return _fullscreen;
    }

    void setMousePosition(uint x, uint y);
    void showMouseCursor(bool show);

    void getSize(uint &w, uint &h);
    static Window *getWindowFromId(Uint32 id);

    void initGUIRender();

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
	//! Devuelve la posición de la ventana
	void getWindowPos(int &x, int &y);
  private:
    WindowHW() : _mainwindow(nullptr),
      _maincontext(nullptr), gotDoubleBuffer(false), gotDebugContext(false), _fullscreen(false) {};
    SDL_Window *_mainwindow;    /* Our window handle */
    SDL_GLContext _maincontext; /* Our opengl context handle */
    std::string reqGLVersion;
    bool gotDoubleBuffer, gotDebugContext, gotStereo;
    bool _fullscreen;

    static std::map<Uint32, Window *> windowIdToWindow;
  };

  class GamepadHW {
  public:
    explicit GamepadHW(size_t gamepad);
    ~GamepadHW();
    std::string getName() const;
    size_t getNumAxes() const;
    size_t getNumButtons() const;
    size_t getNumHats() const;
    size_t getNumBalls() const;
  private:
    SDL_Joystick *gameController;
  };

  class EventSourceHW : public EventSource {
  public:
    bool getEvent(Event &e) override;
  };
}
