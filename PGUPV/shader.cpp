#include <iomanip>

#include "shader.h"
#include "utils.h"
#include "log.h"

using PGUPV::Shader;
using std::string;

/* PARA AÑADIR NUEVOS TIPOS DE SHADER, MODIFICA DESDE AQUí ... */

// Extensiones por defecto de los shaders (si las cambias, usa minúsculas!)
// Su posición y cantidad deben coincidir con las constantes definidas en
// shaders.h
static const string shaderFileExtensions[] = {".vert", ".frag", ".geom",
                                              ".tesc", ".tese"
#ifndef __APPLE__
    , ".comp"
#endif
};

// Nombres de los tipos de shaders. Se usan sólo al imprimir el código fuente
// del shader.
static const string shaderTypeNames[] = {
    "Vertex shader", "Fragment shader", "Geometry shader",
    "Tesselation control shader", "Tessellation evaluation shader"
#ifndef __APPLE__
    ,"Compute shader"
#endif
};

// Constantes de OpenGL. Su posición y cantidad deben coincidir con las
// constantes definidas
// en shaders.h
static const unsigned int glShaderTypeConstants[] = {
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER,
    GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
#ifndef __APPLE__
    , GL_COMPUTE_SHADER
#endif
};

/* ... HASTA AQUí */

#define INCLUDE_STRING "$include"
#define MAX_INCLUDE_LEVELS 8

Shader::Shader() {
	assert(sizeof(shaderFileExtensions) / sizeof(shaderFileExtensions[0]) == NUM_SHADER_TYPES);
	assert(sizeof(shaderTypeNames) / sizeof(shaderTypeNames[0])== NUM_SHADER_TYPES);
	assert(sizeof(glShaderTypeConstants) / sizeof(glShaderTypeConstants[0]) == NUM_SHADER_TYPES);

  shaderId = 0;
  type = CHECK_EXTENSION;
}

Shader::~Shader() { deleteShaderObject(); }

std::shared_ptr<Shader>
Shader::loadFromFile(const string &name, ShaderType shader_type,
                     const std::map<std::string, Strings> &transTable) {
  if (shader_type < CHECK_EXTENSION || shader_type >= NUM_SHADER_TYPES)
    ERRT("Tipo de shader declarado desconocido");

  if (shader_type == CHECK_EXTENSION) {
    shader_type = (ShaderType)check_extension(name);
    if (shader_type < 0)
      ERRT("Tipo de shader desconocido");
  }

  Strings ss;
  if (loadTextFile(name, ss)) {
    Strings preprocessed = preprocessShader(ss);
    std::shared_ptr<Shader> result =
        loadFromMemory(preprocessed, shader_type, transTable);
    result->filename = name;
    result->modificationTime = PGUPV::getFileModificationTime(name);
      
    return result;
  } else
    ERRT("No se ha podido cargar el fichero " + name);
}

std::shared_ptr<Shader>
Shader::loadFromMemory(const Strings &ssrc, ShaderType shader_type,
                       const std::map<std::string, Strings> &transTable) {
  if (shader_type < VERTEX_SHADER || shader_type >= NUM_SHADER_TYPES)
    ERRT("Tipo de shader declarado desconocido");

  std::shared_ptr<Shader> result = std::shared_ptr<Shader>(new Shader());
  result->src = expandText(transTable, ssrc);
  result->type = shader_type;
  result->modificationTime = 0;
  return result;
}

int Shader::check_extension(const string &filename) {
  string ext = PGUPV::to_lower(getExtension(filename));

  for (int i = 0; i < NUM_SHADER_TYPES; i++)
    if (ext == shaderFileExtensions[i])
      return i;
  return -1;
}

// Preprocesa el código fuente del shader cuyo código fuente está en src
Strings Shader::preprocessShader(const Strings &src) {
  Strings input = src;
  Strings output;
  for (uint include_level = 0; include_level < MAX_INCLUDE_LEVELS;
       include_level++) {
    bool any_include = false;
    output.clear();
    for (Strings::const_iterator i = input.begin(); i != input.end(); ++i) {
      if (PGUPV::starts_with(*i, INCLUDE_STRING)) {
        processInclude(*i, output);
        any_include = true;
      } else
        output.push_back(*i);
    }
    if (!any_include)
      return output;
    input = output;
  }
  ERRT("No se permiten más de " + std::to_string(MAX_INCLUDE_LEVELS) +
       " niveles de anidamiento en los $include");
}

void Shader::processInclude(const std::string &line, Strings &dst) {
  // line follows the format $include "<nombre de fichero>"
  std::string::size_type spos = line.find_first_of('\"');
  if (spos != std::string::npos) {
    std::string::size_type epos = line.find_last_of('\"');
    std::string filename = line.substr(spos + 1, epos - spos - 1);
    Strings ss;
    if (!loadTextFile(filename, ss))
      ERRT("No se ha podido cargar el fichero " + filename);
    for (Strings::const_iterator i = ss.begin(); i != ss.end(); ++i) {
      dst.push_back(*i);
    }
  } else
    ERRT("Error en la directiva $include. La sintaxis es: $include "
         "\"<fichero>\"");
}

std::string Shader::getDefaultShaderExtension(ShaderType type) {
  if (type <= CHECK_EXTENSION || type >= NUM_SHADER_TYPES)
    ERRT("Tipo de shader inexistente");
  return shaderFileExtensions[type];
}

void Shader::printSrc(std::ostream &os, bool printHeader,
                      bool printLineNumber) const {
  if (printHeader) {
    os << shaderTypeNames[type] << ": " << filename << std::endl;
    os << string(shaderTypeNames[type].size() + 2 + filename.size(), '-')
       << std::endl;
  }
  uint line = 1;
  for (std::vector<std::string>::const_iterator i = src.cbegin();
       i != src.cend(); ++i, ++line) {
    if (printLineNumber)
      os << std::setw(3) << line << " |";
    os << *i << std::endl;
  }
}

GLuint Shader::compile() {
  INFO("Compilando " + filename);
  errorMsg.clear();

  if (shaderId != 0) {
    INFO("El shader ya estaba compilado");
    return shaderId;
  }
  Strings srccopy;
  srccopy = src;

  unsigned int nlines = (GLsizei)srccopy.size();
  // Añadir saltos de línea al final de cada línea.
  for (unsigned int j = 0; j < nlines; j++)
    srccopy[j].push_back('\n');

  std::vector<GLchar *> ssrc(nlines);

  for (unsigned int j = 0; j < nlines; j++)
    ssrc[j] = (GLchar *)srccopy[j].c_str();

  // Here comes the GL stuff
  CHECK_GL();
  shaderId = glCreateShader(glShaderTypeConstants[type]);
  if (shaderId == 0)
    ERRT("Error ejecutando glCreateShader");

  glShaderSource(shaderId, nlines, (const GLchar **)&ssrc[0], NULL);
  if (glGetError() != GL_NO_ERROR)
    ERRT("Error ejecutando glShaderSource");

  glCompileShader(shaderId);
  GLint compiled;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);

  if (compiled == GL_FALSE) {
    GLint length;
    GLchar *log;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length];
    glGetShaderInfoLog(shaderId, length, &length, log);
    errorMsg = std::string("Error compilando el shader:") + log;
    delete[] log;
    deleteShaderObject();
  }

  return shaderId;
}

std::string Shader::getErrorMessage() const {
  return errorMsg;
}

void Shader::deleteShaderObject() {
  if (shaderId != 0) {
    glDeleteShader(shaderId);
    shaderId = 0;
  }
}

GLint Shader::toGLType(ShaderType type) {
  if (type <= CHECK_EXTENSION || type >= NUM_SHADER_TYPES)
    ERRT("Tipo de shader desconocido");
  return glShaderTypeConstants[type];
}

std::string Shader::toFriendlyName(ShaderType type) {
  if (type <= CHECK_EXTENSION || type >= NUM_SHADER_TYPES)
    ERRT("Tipo de shader desconocido");
  return shaderTypeNames[type];
}