#include <sstream>
#include "glVersion.h"
#include "log.h"

using PGUPV::GLVersion;

GLVersion GLVersion::parse(const std::string &s) {
  std::stringstream ss(s);

  int m1, m2, m3;
  char dot1, dot2;
  ss >> m1 >> dot1 >> m2;
  if (!ss || dot1 != '.') ERRT("No se puede reconocer la versión de GL " + s);
  if (ss.eof()) m3 = 0;
  else {
    ss >> dot2 >> m3;
    if (!ss || dot2 != '.') ERRT("No se puede reconocer la versión de GL " + s);
  }
  return GLVersion(m1, m2, m3);
}

std::string GLVersion::to_string() {
  return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(release);
}
