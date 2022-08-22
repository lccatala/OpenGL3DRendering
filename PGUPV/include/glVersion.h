#pragma once

#include <string>

namespace PGUPV {

#ifndef _WIN32
#undef major
#undef minor
#endif

struct GLVersion {
  int major, minor, release;
  GLVersion(int major, int minor) :
    major(major), minor(minor), release(0) {};

  GLVersion(int major, int minor, int release) :
    major(major), minor(minor), release(release) {};

  /**
  \return true if this >= other
  */
  bool isGreaterEqual(const GLVersion &other) {
    return
      major > other.major ||
      (major == other.major && minor >= other.minor) ||
      (major == other.major && minor == other.minor && release >= other.release);
  }

  /**
  Decodes a string with the format 4.5.0 as major=4 minor=5 release=0
  */
  static GLVersion parse(const std::string &s);
  /**
  \return a string with the format\<major\>.\<minor\>.\<release\>
  */
  std::string to_string();
};
};