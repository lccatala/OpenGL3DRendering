#ifndef _WINDOW_BUILDER 
#define _WINDOW_BUILDER 2013

#include <string>
#include "window.h"

namespace PGUPV {
  class WindowBuilder
  {
  public:
    WindowBuilder(void);
    /**
    Crea una ventana con los siguientes valores por defecto (que se pueden modificar
    llamando a la función set correspondiente:

    _prefMajorVersion = 4
    _prefMinorVersion = 4;
    _minMinorVersion = 0;
    _compatFlag = false;
    _x = 50;
    _y = 50;
    _w = 512;
    _h = 512;
    _flags = PGUPV::PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER;
    _title = "";

    */
    Window *build();
    WindowBuilder &setMinOpenGLVer(uint minor);
    WindowBuilder &setPrefOpenGLVer(uint major, uint minor);
    WindowBuilder &setOpenGLCompatabilityProfile();
    WindowBuilder &setTitle(std::string title);
    WindowBuilder &setFlags(uint flags);
    WindowBuilder &setInitPosition(uint x, uint y);
    WindowBuilder &setInitSize(uint width, uint height);
  private:
    int _prefMajorVersion, _prefMinorVersion;
    int _minMinorVersion;
    bool _compatFlag;
    uint _x, _y;
    uint _w, _h;
    uint _flags;
    std::string _title;
  };

};

#endif