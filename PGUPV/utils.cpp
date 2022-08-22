
#include <cctype>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string.h>
#ifdef _WIN32
#pragma warning( push)
#pragma warning( disable : 4505)
#endif
#include <dirent.h>
#ifdef _WIN32
#pragma warning(pop)
#endif

#include <regex>
#include <string>
#include <cstdio>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#ifdef __APPLE__
#include <spawn.h>
extern char **environ;
#elif _WIN32
#include <process.h>
#else
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifdef _MSC_VER
#include <tchar.h>
#include <windows.h>
#endif

#include <glm/glm.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>

#include "utils.h"
#include "log.h"
#include "glVersion.h"

#include <glm/gtc/matrix_transform.hpp>

using std::string;
using PGUPV::getExtension;
using PGUPV::loadTextFile;
using PGUPV::decodeGLError;
using PGUPV::GLVersion;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

std::string PGUPV::getExtension(const std::string &filename) {
  string::size_type dot;

  dot = filename.find_last_of('.');

  if (dot == string::npos)
    return "";

  return filename.substr(dot);
}

bool PGUPV::loadTextFile(const std::string &filename, Strings &content) {
  std::ifstream file;

  content.clear();
  file.open(filename.c_str());
  if (!file)
    return false;

  while (!file.eof()) {
    string st;
    getline(file, st);
    content.push_back(st);
  }
  file.close();
  return true;
}

string PGUPV::decodeGLError(GLenum err) {
  switch (err) {
  case GL_INVALID_ENUM:
    return string("Invalid enum");
    break;
  case GL_INVALID_VALUE:
    return string("Invalid value");
    break;
  case GL_INVALID_OPERATION:
    return string("Invalid operation");
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return string("Invalid framebuffer operation");
    break;
  case GL_OUT_OF_MEMORY:
    return string("Out of memory");
    break;
  case GL_NO_ERROR:
    return string("No error");
    break;
  default:
    return string("Unknown error code");
    break;
  }
}

void PGUPV::checkGLState(std::string filename, int line, std::string msg) {
  GLenum err;
  string message;
  // glGetError puede acumular más de un error: mostrarlos todos en la ventana
  // del depurador, y devolver en la
  // excepción el último
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::stringstream o;
    if (msg.length() > 0) {
      o << msg << std::endl;
      msg = "";
    }
    o << filename << " (" << line << "): "
      << "El estado de OpenGL es erróneo: " + decodeGLError(err) << std::endl;

    message = o.str();

#ifdef _DEBUG
#ifdef _MSC_VER
    OutputDebugStringA(message.c_str());
    // Al ejecutar la siguiente instrucción, si hay un depurador en ejecución,
    // es como si se hubiera puesto un punto
    // de ruptura. Ahora puedes ver la pila de llamadas para ver el código desde
    // donde se ha llamado a esta función.
    __debugbreak();
#else
    std::cerr << message << std::endl;
    raise(SIGTRAP);
#endif
#endif
  }
  if (!message.empty()) {
    ERRT(message);
  }
}

glm::vec3 *PGUPV::apply(const glm::mat3 &m, const glm::vec3 *v, uint n) {
  // Crea una copia de los vértices, aplicando la transformación indicada
  vec3 *vt = new vec3[n];
  for (uint i = 0; i < n; i++)
    vt[i] = m * v[i];
  return vt;
}

glm::vec4 *PGUPV::apply(const glm::mat4 &m, const glm::vec4 *v, uint n) {
  // Crea una copia de los vértices, aplicando la transformación indicada
  vec4 *vt = new vec4[n];
  for (uint i = 0; i < n; i++)
    vt[i] = m * v[i];
  return vt;
}

glm::vec3 *PGUPV::apply(const glm::mat4 &m, const glm::vec3 *v, uint n) {
  // Crea una copia de los vértices, aplicando la transformación indicada
  vec3 *vt = new vec3[n];
  for (uint i = 0; i < n; i++) {
    vec4 tt = m * vec4(v[i], 1.0f);
    vt[i] = (tt.w == 0.0) ? (vec3)tt : ((vec3)tt) / tt.w;
  }
  return vt;
}

glm::vec2 *PGUPV::apply(const glm::mat4 &m, const glm::vec2 *v, uint n) {
  // Crea una copia de los vértices, aplicando la transformación indicada
  vec2 *vt = new vec2[n];
  for (uint i = 0; i < n; i++) {
    vec4 tt = m * vec4(v[i], 0.0f, 1.0f);
    vt[i] = (tt.w == 0.0) ? (vec2)tt : ((vec2)tt) / tt.w;
  }
  return vt;
}

float *PGUPV::apply(const glm::mat3 &m, const float *v, uint n) {
  // Crea una copia de los vértices, aplicando la transformación indicada
  float *vt = new float[3 * n];
  for (uint i = 0; i < n; i++) {
    vec3 p;
    for (uint j = 0; j < 3; j++) {
      p[j] = v[i * 3 + j];
    }
    vec3 tp = m * p;
    for (uint j = 0; j < 3; j++) {
      vt[i * 3 + j] = tp[j];
    }
  }
  return vt;
}

// Devuelve la fecha y hora actuales
long long PGUPV::getCurrentTime() {
  long long t;
#ifdef _WIN32
  __time64_t long_time;

  // Get time as 64-bit integer.
  _time64(&long_time);
  t = long_time;
#else
  time_t long_time;

  time(&long_time);
  t = long_time;
#endif
  return t;
}

long long PGUPV::getFileModificationTime(const std::string &pathname) {
  struct stat st;
  int ret = stat(pathname.c_str(), &st);
  if (ret == -1) {
    ERRT("Error obteniendo información del fichero " + pathname);
  }
#ifndef  __APPLE__
  return st.st_mtime;
#else
  return st.st_mtimespec.tv_sec;
#endif
}


std::string PGUPV::getCurrentDateTimeString() {

  char buffer[40];
#ifdef _WIN32
  struct tm newtime;
  __time64_t long_time;

  // Get time as 64-bit integer.
  _time64(&long_time);
  _localtime64_s(&newtime, &long_time);
  asctime_s(buffer, sizeof(buffer), &newtime);
#else
  time_t long_time;

  time(&long_time);
  ctime_r(&long_time, buffer);
#endif
  char *remove = buffer + strlen(buffer) - 1;
  // remove the \n at the end
  while (remove >= buffer && *remove < ' ') {
    *remove = 0;
    remove--;
  }
  return std::string(buffer);
}

std::string PGUPV::getTimeStamp() {

  struct tm newtime;
#ifdef _WIN32
  __time64_t long_time;

  // Get time as 64-bit integer.
  _time64(&long_time);
  _localtime64_s(&newtime, &long_time);

#else
  time_t long_time;

  time(&long_time);
  localtime_r(&long_time, &newtime);
#endif
  std::ostringstream os;
  os << newtime.tm_year + 1900 << std::setw(2) << std::setfill('0')
    << newtime.tm_mon + 1 << std::setw(2) << newtime.tm_mday;
  os << "-" << std::setw(2) << std::setfill('0') << newtime.tm_hour
    << std::setw(2) << newtime.tm_min << std::setw(2) << newtime.tm_sec;
  os << std::setw(3) << std::setfill('0')
    << (((ulong)clock()) * 1000 / CLOCKS_PER_SEC) % 1000;
  return os.str();
}

std::string PGUPV::buildFrameName(std::string base, const ulong number,
  std::string extension) {
  std::ostringstream os;

  os << base;
  os << std::setw(8) << std::setfill('0') << number;
  os << "." << extension;
  return os.str();
}

std::string PGUPV::hexString(ulong n) {
  std::ostringstream os;
  os << "0x" << std::hex << n;
  return os.str();
}

std::string PGUPV::getFilenameFromPath(const std::string &filepath, bool includeExtension) {
  std::string::size_type slash1, slash2, slash;

  slash1 = filepath.find_last_of('\\');
  slash2 = filepath.find_last_of('/');

  if (slash1 == std::string::npos)
    slash = slash2;
  else if (slash2 == std::string::npos)
    slash = slash1;
  else if (slash1 > slash2)
    slash = slash1;
  else
    slash = slash2;

  std::string result;
  if (slash == std::string::npos)
	  result = filepath;
  else 
	result = filepath.substr(slash + 1);

  if (includeExtension) {
	  return result;
  }
  else {
	  std::string::size_type dot;
	  dot = result.find_last_of('.');
	  if (dot == std::string::npos)
		  return result;

	  return result.substr(0, dot);
  }
}

std::string PGUPV::getDirectory(const std::string &filepath) {
  std::string::size_type slash1, slash2, slash;

  slash1 = filepath.find_last_of('\\');
  slash2 = filepath.find_last_of('/');

  if (slash1 == std::string::npos)
    slash = slash2;
  else if (slash2 == std::string::npos)
    slash = slash1;
  else if (slash1 > slash2)
    slash = slash1;
  else
    slash = slash2;

  if (slash == std::string::npos)
    return "";

  return filepath.substr(0, slash + 1);
}

float PGUPV::distSquare(glm::vec3 a, glm::vec3 b) {
  glm::vec3 d = a - b;
  return glm::dot(d, d);
};

bool PGUPV::fileExists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

bool PGUPV::dirExists(const std::string &path)
{
	struct stat info;

	int statRC = stat(path.c_str(), &info);
	if (statRC != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

std::string PGUPV::to_lower(const std::string &s) {
  return boost::algorithm::to_lower_copy(s);
}

/**
Devuelve si la cadena s empieza por b
*/
bool PGUPV::starts_with(const std::string &s, const std::string &b) {
  return s.compare(0, b.length(), b) == 0;
}

/**
Devuelve si la cadena s acaba por b
*/
bool PGUPV::ends_with(const std::string &s, const std::string &b) {
  if (s.length() < b.length()) return 0;
  return s.compare(s.length() - b.length(), b.length(), b) == 0;
}

// Devuelve <posicion del $, longitud del nombre de la variable
std::pair<size_t, size_t> searchVariable(const std::string &line) {
  std::pair<size_t, size_t> res;

  res.first = line.find('$');
  if (res.first == std::string::npos)
    return res;
  size_t i;
  for (i = res.first + 1; i < line.size(); i++)
    if (!isalnum(line[i]) && line[i] != '_')
      break;
  res.second = i - res.first;
  return res;
}

Strings PGUPV::expandText(const std::map<std::string, Strings> &transTable,
  const Strings &org) {
  Strings dst;

  Strings::size_type otam = org.size();
  for (uint i = 0; i < otam; i++) {
    string line = org[i];
    auto se = searchVariable(line);
    if (se.first == std::string::npos)
      // Not found: just copy the line
      dst.push_back(org[i]);
    else {
      // Found a variable! substitute the variable with the value
      auto var = line.substr(se.first, se.second);
      auto pair = transTable.find(var);
      if (transTable.end() == pair)
        ERRT("No se reconoce la variable " + var);
      const Strings &values = pair->second; // subStrings[line];

      auto prefix = line.substr(0, se.first);
      auto suffix = line.substr(se.first + se.second);
      if (values.size() <= 1) {
        auto tmpStr = prefix;
        if (values.size() == 1) tmpStr += values[0];
        tmpStr += suffix;
        dst.push_back(tmpStr);
      }
      else {
        if (prefix.size() > 0) dst.push_back(prefix);
        for (uint j = 0; j < values.size(); j++)
          dst.push_back(values[j]);
        if (suffix.size() > 0)
          dst.push_back(suffix);
      }
    }
  }
  return dst;
}

void PGUPV::trim(std::string &s) { boost::trim(s); }

std::string PGUPV::to_string(float f, uint ndecimals) {
  std::ostringstream ss;
  ss << std::setprecision(ndecimals) << f;
  return ss.str();
}

std::string PGUPV::to_string(const glm::vec4 &v, uint ndecimals) {
  std::ostringstream ss;
  ss << std::setprecision(ndecimals) << "(" << v.x << ", " << v.y << ", "
    << v.z << ", " << v.w << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::vec3 &v, uint ndecimals) {
  std::ostringstream ss;
  ss << std::setprecision(ndecimals) << "(" << v.x << ", " << v.y << ", "
    << v.z << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::vec2 &v, uint ndecimals) {
  std::ostringstream ss;
  ss << std::setprecision(ndecimals) << "(" << v.x << ", " << v.y << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::ivec4 &v) {
  std::ostringstream ss;
  ss << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::ivec3 &v) {
  std::ostringstream ss;
  ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::ivec2 &v) {
  std::ostringstream ss;
  ss << "(" << v.x << ", " << v.y << ")";
  return ss.str();
}

std::string PGUPV::to_string(const glm::mat3 &m, uint ndecimals) {
  std::ostringstream ss;

  ss << std::setprecision(ndecimals);
  for (uint row = 0; row < 3; row++) {
    for (uint col = 0; col < 3; col++) {
      ss << std::setw(ndecimals + 3) << m[col][row] << (col < 2 ? ", " : "");
    }
    ss << std::endl;
  }
  return ss.str();
}

std::string PGUPV::to_string(const glm::mat4 &m, uint ndecimals) {
  std::ostringstream ss;

  ss << std::setprecision(ndecimals);
  for (uint row = 0; row < 4; row++) {
    for (uint col = 0; col < 4; col++) {
      ss << std::setw(ndecimals + 3) << m[col][row] << (col < 3 ? ", " : "");
    }
    ss << std::endl;
  }
  return ss.str();
}

bool PGUPV::isGLExtensionAvailable(const std::string &name) {
  GLint num = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &num);
  for (int i = 0; i < num; i++) {
    if (!name.compare((const char *)glGetStringi(GL_EXTENSIONS, i))) {
      return true;
    }
  }
  return false;
}


uint PGUPV::getNumAvailableExtensions() {
  GLint num = 0;
  glGetIntegerv(GL_NUM_EXTENSIONS, &num);
  return num;
}

void PGUPV::printAvailableGLExtensions(std::ostream &o) {
  uint num = getNumAvailableExtensions();
  for (uint i = 0; i < num; i++) {
    o << (const char *)glGetStringi(GL_EXTENSIONS, i) << " ";
  }
}


#define BYTES_PER_LINE 16

void hexLine(uchar *buffer, uint size, ulong offset, std::ostream &output) {
  output << std::setw(7) << std::setbase(10) << offset;
  output << std::hex;
  for (uint i = 0; i < size; i++) {
    output << " " << std::setw(2) << (int)buffer[i];
  }
  output << std::endl;
}

void PGUPV::hexdump(uchar *buffer, uint size, std::ostream &output) {
  ulong offset = 0;
  while (size >= BYTES_PER_LINE) {
    hexLine(buffer, BYTES_PER_LINE, offset, output);
    buffer += BYTES_PER_LINE;
    offset += BYTES_PER_LINE;
    size -= BYTES_PER_LINE;
  }
  if (size > 0) {
    hexLine(buffer, size, offset, output);
  }
}

PGUPV::Manufacturer PGUPV::getManufacturer() {
  static PGUPV::Manufacturer result;
  static std::string mfg_string;

  if (mfg_string.length() == 0) {
    const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    if (vendor == nullptr)
      return Manufacturer::UNKNOWN;

    mfg_string = std::string(vendor);
    if (std::string::npos != mfg_string.find("NVIDIA"))
      result = Manufacturer::NVIDIA;
    else if (std::string::npos != mfg_string.find("ATI") ||
      std::string::npos != mfg_string.find("ATI"))
      result = Manufacturer::AMD;
    else if (std::string::npos != mfg_string.find("Intel"))
      result = Manufacturer::INTEL;
    else
      result = Manufacturer::UNKNOWN;
  }
  return result;
}


// http://stackoverflow.com/questions/3300419/file-name-matching-with-wildcard

void EscapeRegex(string &regex);

bool MatchTextWithWildcards(const string &text, string wildcardPattern, bool caseSensitive)
{
  // Escape all regex special chars
  EscapeRegex(wildcardPattern);

  // Convert chars '*?' back to their regex equivalents
  boost::replace_all(wildcardPattern, "\\?", string("."));
  boost::replace_all(wildcardPattern, "\\*", ".*");

  auto flags = std::regex::basic;
  if (!caseSensitive) flags |= std::regex::icase;

  std::regex pattern(wildcardPattern, flags);

  return regex_match(text, pattern);
}

void EscapeRegex(string &regex)
{
  boost::replace_all(regex, "\\", "\\\\");
  boost::replace_all(regex, "^", "\\^");
  boost::replace_all(regex, ".", "\\.");
  boost::replace_all(regex, "$", "\\$");
  boost::replace_all(regex, "|", "\\|");
  boost::replace_all(regex, "(", "\\(");
  boost::replace_all(regex, ")", "\\)");
  boost::replace_all(regex, "[", "\\[");
  boost::replace_all(regex, "]", "\\]");
  boost::replace_all(regex, "*", "\\*");
  boost::replace_all(regex, "+", "\\+");
  boost::replace_all(regex, "?", "\\?");
  boost::replace_all(regex, "/", "\\/");
}

std::vector<std::string> PGUPV::listFiles(const std::string &path, bool recursive) {
  return listFiles(path, recursive, std::vector<std::string>());
}

std::vector<std::string> PGUPV::listDirs(const std::string & path)
{
  DIR *dir;
  struct dirent *ent;
  std::vector<std::string> results;

  std::string wpath = path;
  if (path.empty())
    wpath = "./";
  else
    if (path.back() != '/' && path.back() != '\\')
      wpath += "/";

  /* Open directory stream */
  dir = opendir(wpath.c_str());
  if (dir == NULL) {
    ERRT("No se puede abrir el directorio " + wpath);
  }

  /* Print all files and directories within the directory */
  while ((ent = readdir(dir)) != NULL) {
    switch (ent->d_type) {
    case DT_DIR:
      if (std::string(".") != ent->d_name && std::string("..") != ent->d_name) {
        results.push_back(ent->d_name);
      }
      break;
    }
  }
  closedir(dir);
  return results;
}

std::vector<std::string> PGUPV::listFiles(const std::string &path, bool recursive, const std::vector<std::string> &patterns) {
  DIR *dir;
  struct dirent *ent;
  std::vector<std::string> results;

  std::string wpath = path;
  if (path.empty())
    wpath = "./";
  else
    if (path.back() != '/' && path.back() != '\\')
      wpath += "/";

  /* Open directory stream */
  dir = opendir(wpath.c_str());
  if (dir != NULL) {

    /* Print all files and directories within the directory */
    while ((ent = readdir(dir)) != NULL) {
      switch (ent->d_type) {
      case DT_REG:
        if (patterns.empty()) results.push_back(wpath + ent->d_name);
        else {
          for (auto p : patterns) {
            if (MatchTextWithWildcards(ent->d_name, p, getPlatform() != Platform::WIN)) {
              results.push_back(wpath + ent->d_name);
              break;
            }
          }
        }
        break;

      case DT_DIR:
        if (std::string(".") != ent->d_name && std::string("..") != ent->d_name && recursive) {
          auto tmp = listFiles(wpath + ent->d_name, true, patterns);
          std::move(tmp.begin(), tmp.end(), std::back_inserter(results));
        }
        break;
      }
    }
    closedir(dir);
  }
  else {
    /* Could not open directory */
    ERRT("No se puede abrir el directorio " + wpath);
  }
  return results;
}

typedef unsigned value_type;

template <typename Iterator>
size_t get_length(Iterator p)
{
  unsigned char c = static_cast<unsigned char> (*p);
  if (c < 0x80) return 1;
  else if (!(c & 0x20)) return 2;
  else if (!(c & 0x10)) return 3;
  else if (!(c & 0x08)) return 4;
  else if (!(c & 0x04)) return 5;
  else return 6;
}

template <typename Iterator>
value_type get_value(Iterator p)
{
  size_t len = get_length(p);

  if (len == 1)
    return *p;

  value_type res = static_cast<unsigned char> (
    *p & (0xff >> (len + 1)))
    << ((len - 1) * 6);

  for (--len; len; --len)
    res |= (static_cast<unsigned char> (*(++p)) - 0x80) << ((len - 1) * 6);

  return res;
}

std::string PGUPV::utf8ToLatin1(std::string utf8) {
  std::string s_latin1;
  bool outOfBounds = false;
  for (std::string::iterator p = utf8.begin(); p != utf8.end(); ++p)
  {
    value_type value = get_value<std::string::iterator&>(p);
    if (value > 0xff) {
      if (!outOfBounds) {
        ERR("Error conviertiendo de utf8 a latin1: '" + utf8 + "'. Recuerda guardar tu código fuente en utf8");
        outOfBounds = true;
      }
    }
    else
      s_latin1 += static_cast<std::string::value_type>(value);
  }
  return s_latin1;
}

std::string PGUPV::to_string(HatPosition pos) {
  switch (pos) {
  case LeftUp:
    return "LeftUp";
    break;
  case Up:
    return "Up";
    break;
  case RightUp:
    return "RightUp";
    break;
  case Left:
    return "Left";
    break;
  case Centered:
    return "Centered";
    break;
  case Right:
    return "Right";
    break;
  case LeftDown:
    return "LeftDown";
    break;
  case Down:
    return "Down";
    break;
  case RightDown:
    return "RightDown";
    break;
  }
  ERRT("Unkwnown code");
}

int PGUPV::execute(std::string filename, std::vector<std::string> args) {
  std::string pathname;

  const char **argv = new const char *[args.size() + 2];

#ifndef _WIN32
  std::string tmp = filename;
  size_t w = tmp.rfind(".exe");
  size_t l = tmp.length();
  if (w == l - 4) {
    tmp = filename;
    tmp.resize(l - 4);
  }
  pathname += tmp;
  argv[0] = tmp.c_str();
#else
  argv[0] = filename.c_str();
  pathname += filename;
#endif

  int offset = 1;
  for (auto &arg : args) {
    argv[offset++] = arg.c_str();
  }
  argv[offset] = NULL;
#ifndef _WIN32
  pid_t pid;
  std::cerr << getcwd(nullptr, 0) << std::endl;
  std::cerr << pathname << std::endl;
  for (uint i = 0; i < args.size() + 1; i++)
    std::cerr << i << " -> " << argv[i] << std::endl;
  int res = posix_spawn(&pid, pathname.c_str(), nullptr, nullptr,
    (char *const *)argv, environ);
  if (res == 0)
    std::cerr << "PROCESO LANZADO\n";
  else {
    std::cerr << "FALLO AL LANZAR EL PROCESO\n";
    return -120;
  }
  int stat_loc;
  waitpid(pid, &stat_loc, 0);
  if (WIFEXITED(stat_loc)) {
    return WEXITSTATUS(stat_loc);
  }
  else {
    std::cerr << "Error esperando al proceso lanzado" << std::endl;
    return -120;
  }
#else
  auto res = _spawnv(P_WAIT, pathname.c_str(), argv);
  delete[]argv;
  return static_cast<int>(res);
#endif
}


bool PGUPV::deleteFile(const std::string &filename) {
#ifndef _WIN32
#define _unlink unlink
#endif
  return _unlink(filename.c_str()) == 0;
}

std::string PGUPV::getCurrentWorkingDir() {
#ifndef _WIN32
#define _getcwd getcwd
#endif
  char path[1024];

  if (_getcwd(path, sizeof(path)) == nullptr) ERRT("Error al obtener el directorio actual");

  std::string res = path;
  if (!ends_with(res, "/") && !ends_with(res, "\\"))
    res += "/";
  return res;
}


std::string PGUPV::removeDotAndDotDot(const std::string &path) {
	std::vector<std::string> tokens;
	size_t offset = 0, found;
	
	auto input = path;
	PGUPV::trim(input);

	while (offset < input.length()) {
		found = input.find_first_of("/\\", offset);
		if (found == input.npos) {
			found = input.length();
		}
		auto substr = input.substr(offset, found - offset);
		if (!substr.empty()) {
			if (substr == "..") {
				if (tokens.empty()) {
					ERR("Path no válido: " + input);
					return input;
				}
				else tokens.pop_back();
			}
			else if (substr != ".") {
				tokens.push_back(substr);
			}
		}
		offset = found + 1;
	}

	std::ostringstream os;

	std::string sep;
	if (PGUPV::getPlatform() == PGUPV::Platform::WIN) {
		sep = "\\";
	}
	else {
		sep = "/";
		os << sep;
	}

	if (!tokens.empty()) {
		if (sep == "/") os << sep;
		for (size_t i = 0; i < tokens.size() - 1; i++) {
			os << tokens[i] << sep;
		}
		os << tokens.back();
	}
	return os.str();
}

bool PGUPV::isAbsolutePath(const std::string &path) {
	auto input = path;
	trim(input);
	if (PGUPV::getPlatform() == PGUPV::Platform::WIN) {
		return (input.length() >= 2 && input[1] == ':' && input[2] == '\\') || PGUPV::starts_with(input, "\\\\");
	}
	else {
		return PGUPV::starts_with(input, "/");
	}
}


bool PGUPV::isRelativePath(const std::string &path) {
	return !isAbsolutePath(path);
}


std::string PGUPV::relativeToAbsolute(const std::string & relative)
{
	return removeDotAndDotDot(getCurrentWorkingDir() + relative);
}

void PGUPV::changeCurrentDir(const std::string &dir) {
#ifndef _WIN32
#define _chdir chdir
#endif
  if (_chdir(dir.c_str()))
    ERRT("Error al cambiar el directorio actual a " + dir);
}

bool PGUPV::createDir(const std::string & dir)
{
#ifdef _WIN32
	return _mkdir(dir.c_str()) == 0;
#else
	return mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif
}

int PGUPV::strcpy_s(char * strDestination, size_t numberOfElements, const char * strSource)
{
#ifdef _WIN32
  return ::strcpy_s(strDestination, numberOfElements, strSource);
#else
  for (unsigned int i = 0; i < numberOfElements - 1 && *strSource; i++) {
    *strDestination++ = *strSource++;
  }
  *strDestination = 0;
  return 0;
#endif
}

std::string PGUPV::removeExtension(const std::string &filename) {
  string::size_type dot;

  dot = filename.find_last_of('.');

  if (dot == filename.npos)
    return filename;

  if (filename.find('/', dot) != filename.npos || filename.find('\\', dot) != filename.npos)
    return filename;

  return filename.substr(0, dot);
}

bool PGUPV::containsIgnoreCase(const std::string &dondeBuscar, const std::string &queBuscar)
{
  auto it = std::search(
    dondeBuscar.begin(), dondeBuscar.end(),
    queBuscar.begin(), queBuscar.end(),
    [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
  );
  return (it != dondeBuscar.end());
}


bool PGUPV::extractTRSfromMatrix(const glm::mat4 mat, glm::vec3 &trans, glm::mat4 &rot, glm::vec3 &scale) {
  glm::mat4 locmat;
  /* Vector4 type and functions need to be added to the common set. */
  glm::vec3 row[3];

  locmat = mat;
  /* Normalize the matrix. */
  if (locmat[3][3] == 0)
    return false;
  locmat /= locmat[3][3];

  /* Clear the perspective partition. */
  locmat[0][3] = locmat[1][3] = locmat[2][3] = 0.0f;
  locmat[3][3] = 1.0f;

  trans = glm::vec3(locmat[3]);

  /* Now get scale and shear. */
  for (int i = 0; i < 3; i++) {
    row[i] = glm::vec3(locmat[i]);
  }

  /* Compute X scale factor and normalize first row. */
  scale.x = glm::length(row[0]);
  row[0] /= scale.x;

  /* Now, compute Y scale and normalize 2nd row. */
  scale.y = glm::length(row[1]);
  row[1] /= scale.y;

  /* Next, get Z scale and normalize 3rd row. */
  scale.z = glm::length(row[2]);
  row[2] /= scale.z;

  if (fabs(glm::dot(row[0], row[1])) > 1e-5 ||
    fabs(glm::dot(row[1], row[2])) > 1e-5 ||
    fabs(glm::dot(row[0], row[2])) > 1e-5)
    return false; // Había shear

    /* At this point, the matrix (in rows[]) is orthonormal.
     * Check for a coordinate system flip.  If the determinant
     * is -1, then negate the matrix and the scaling factors.
     */
  if (glm::dot(row[0], glm::cross(row[1], row[2])) < 0) {
    scale *= -1.0f;
    for (int i = 0; i < 3; i++) {
      row[i] *= -1;
    }
  }
  rot = glm::mat4(glm::mat3(row[0], row[1], row[2]));
  return true;
}


glm::mat4 PGUPV::removeScaling(const glm::mat4 &m) {
  glm::mat4 res = m;

  for (glm::vec3::length_type i = 0; i < 3; i++) {
    glm::vec3 t = glm::vec3(m[i]);
    float l = glm::length(t);
    for (glm::mat4::length_type j = 0; j < 3; j++)
      res[i][j] /= l;
  }

  return res;
}


void PGUPV::renameOrMove(const std::string &from, const std::string &to) {
  if (std::rename(from.c_str(), to.c_str()))
    ERRT("Error al intentar cambiar el nombre a " + from);
}


void PGUPV::repetitionsInFile(const std::string &filename, const std::string &str) {
  std::ifstream infile(filename);

  std::string line;
  int occurrences = 0;
  while (std::getline(infile, line))
  {
    string::size_type start = 0;
    while ((start = line.find(str, start)) != string::npos) {
      ++occurrences;
      start += str.length();
    }
  }


}