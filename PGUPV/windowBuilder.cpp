#include <sstream>
#include <glm/glm.hpp>

#include "app.h"
#include "log.h"
#include "windowBuilder.h"

using PGUPV::WindowBuilder;
using PGUPV::Window;

WindowBuilder::WindowBuilder(void) :
_prefMajorVersion(4), _prefMinorVersion(5), _minMinorVersion(0),
_compatFlag(false), _x(50), _y(50), _w(512), _h(512),
_flags(PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER)
{
}

WindowBuilder &WindowBuilder::setMinOpenGLVer(uint minor) {
  _minMinorVersion = minor;
  return *this;
}

WindowBuilder &WindowBuilder::setPrefOpenGLVer(uint major, uint minor) {
  if (major > 1000000 || minor > 1000000)
    ERRT("Versiones de OpenGL erróneas");
  _prefMajorVersion = major;
  _prefMinorVersion = minor;
  return *this;
}

WindowBuilder &WindowBuilder::setOpenGLCompatabilityProfile() {
  _compatFlag = true;
  return *this;
}
WindowBuilder &WindowBuilder::setTitle(std::string title) {
  _title = title;
  return *this;
}

WindowBuilder &WindowBuilder::setFlags(uint flags) {
  _flags = flags;
  return *this;
}

WindowBuilder &WindowBuilder::setInitPosition(uint x, uint y) {
  _x = x;
  _y = y;
  return *this;
}

WindowBuilder &WindowBuilder::setInitSize(uint width, uint height) {
  _w = width;
  _h = height;
  return *this;
}

Window *WindowBuilder::build() {
  Window *window;

  if (_minMinorVersion > _prefMinorVersion) {
    WARN("La mínima versión de OpenGL pedida es mayor que la preferida.");
    _minMinorVersion = _prefMinorVersion;
  }

  window = new Window();
  if (_compatFlag) {
    window->setOpenGLCompatibilityFlag();
  }
  for (int minor = _prefMinorVersion; minor >= _minMinorVersion; minor--) {
    window->setRequiredOpenGLVersion(_prefMajorVersion, minor);
    if (window->createWindow(_title, _flags, _x, _y, _w, _h)) {
      std::ostringstream oss;
      oss << "Información del driver:" << std::endl;
      window->printInfo(oss);
      INFO(oss.str());
      return window;
	}
	else {
		// if this happens, there is something wrong. Clear the property
		PGUPV::App::getInstance().removeProperty(PGUPV::App::PROP_HIGHEST_GL_VERSION_SEEN);
	}
  }
  delete window;
  return NULL;
}
