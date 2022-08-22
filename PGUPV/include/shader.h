#ifndef _SHADER_H 
#define _SHADER_H 2014

#include <string>
#include <map>
#include <memory>
#include <GL/glew.h>
#include "common.h"

namespace PGUPV {

  /**
  \class Shader

  Esta clase representa un Shader, almacenado normalmente en un fichero. Este shader será de
  alguno de los tipos conocidos (vértice, fragmento, geometría, etc).
  Esta clase trabaja conjuntamente con la clase PGUPV::Program

  */
  class Shader {
  public:
    ~Shader();

    /**
    Tipos de shaders reconocidos por la librería
    Para añadir nuevos tipos de shaders, mira en shader.cpp
    */
    enum ShaderType {
      CHECK_EXTENSION = -1, VERTEX_SHADER = 0, FRAGMENT_SHADER, GEOMETRY_SHADER,
      TESS_CONTROL_SHADER, TESS_EVALUATION_SHADER
#ifndef __APPLE__
        , COMPUTE_SHADER
#endif
        , NUM_SHADER_TYPES
    };

    /**
     Carga el shader almacenado en el nombre de fichero dado. Se puede especificar el tipo
     de shader, o dejar que la librería lo averigue por la extensión.
     En el proceso de carga, se usará la tabla de traducciones indicada para sustituir texto dentro del shader (una
     especie de pasada de preprocesador). Una vez cargado, el código fuente del shader no se puede cambiar.
     Si no se puede cargar, lanza una excepción.
     \param name Ruta del fichero que contiene el código fuente del shader
     \param shader_type Tipo del shader que se está cargando. Por defecto, intentará averiguar el tipo de shader por
     la extensión del fichero
     \param transTable Tabla de traducciones. Cada línea del shader que coincida con un elemento del índice del mapa,
     se sustituirá por el conjunto de cadenas asociadas a dicho elemento del mapa
     */

    static std::shared_ptr<Shader> loadFromFile(const std::string &name,
      ShaderType shader_type = CHECK_EXTENSION,
      const std::map<std::string, Strings> &transTable = std::map<std::string, Strings>());

    /**
    Cargar un shader desde memoria
    Si no se puede cargar, lanza una excepción
    \param ssrc El código fuente del shader
    \param shader_type Tipo del shader que se está cargando
    \param transTable Tabla de traducciones. Cada línea del shader que coincida con un elemento del índice del mapa,
    se sustituirá por el conjunto de cadenas asociadas a dicho elemento del mapa
    */
    static std::shared_ptr<Shader> loadFromMemory(const Strings &ssrc,
      ShaderType shader_type,
      const std::map<std::string, Strings> &transTable = std::map<std::string, Strings>());
    /**
    \return el tipo del shader
    */
    ShaderType getType() const { return type; };
    /**
    \return el fichero desde donde se cargó (o la cadena vacía si se cargó desde memoria)
    */
    std::string getFilename()  const { return filename; };

    /**
    Devuelve la extensión de fichero por defecto del tipo de shader indicado.
    \return Una cadena con la extensión (por ejemplo, ".vert")
    */
    static std::string getDefaultShaderExtension(ShaderType type);

    /**
     Imprime el código fuente del shader. Para imprimir todos los shaders cargados, ver el método \sa{Program::printSrcs}
     \param os Flujo de salida donde escribir el código fuente del shader
     \param printHeader Si true, imprime el nombre del fichero y el tipo de shader
     \param printLineNumber si true, precede cada línea del código fuente con su número de línea
     */
    void printSrc(std::ostream &os, bool printHeader = true, bool printLineNumber = true) const;
    /**
      Compila el código fuente del shader proporcionado (si es necesario). Si se producen errores de compilación
      se mandan al Log.
      \return El identificador del shader compilado, o 0 si se produce algún error
      */
    GLuint compile();
    
    /**
    En el caso de que la compilación haya fallado, se puede recuperar el error de compilación mediante esta función.
    */
    std::string getErrorMessage() const;
    
    /**
    Si el shader está compilado, elimina su código (el shader se puede volver a recompilar para recrearlo)
    */
    void deleteShaderObject();
    /**
    Traduce desde el tipo ShaderType a la constante correspondiente de GL. Por ejemplo:
    toGLType(Shader::VERTEX_SHADER) -> GL_VERTEX_SHADER
    */
    static GLint toGLType(ShaderType type);

    /**
    Traduce desde el tipo de ShaderType a un nombre entendible. Por ejemplo:
    toFriendlyName(Shader::VERTEX_SHADER) -> "Vertex shader"
    */
    static std::string toFriendlyName(ShaderType type);
      
    /** 
     \return La fecha de modificación del shader. Número de segundos desde 1/1/1970. Es 0 si 
       el shader se cargó desde memoria
     */
      long long getModificationTime() { return modificationTime; }

  private:
    Shader();	// Usar las funciones factoría para crear un shader
    /**
    \return el tipo de shader que coincide con alguna extensión de las de arriba,
    o -1 si no coincide con ninguna extensión.
    */
    static int check_extension(const std::string &filename);

    /**
    Preprocesa el código fuente del shader cuyo código fuente está en src
    */
    static Strings preprocessShader(const Strings &src);
    /**
    Procesa una directiva $include, abriendo el fichero y escribiendo en contenido
    al final del vector de cadenas indicado
    */
    static void processInclude(const std::string &line, Strings &dst);

    Strings src; // Código fuente
    std::string filename; // Nombre del fichero desde donde se cargó (vacío si se cargó desde memoria)
    ShaderType type; // Tipo de shader
    GLuint shaderId;
    long long modificationTime; // Fecha de modificación del fichero (o 0 si se cargó desde memoria)
    std::string errorMsg; // Mensajes de error generados durante la compilación
  };
};
#endif
