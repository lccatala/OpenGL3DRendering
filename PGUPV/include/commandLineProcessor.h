#ifndef _COMMANDLINEPROCESSOR_H 
#define _COMMANDLINEPROCESSOR_H 2013

#include <list>
#include <string>

namespace PGUPV {
  class App;

  /**
  \class CommandLineProcessor

  Se encarga de procesar la lista de parámetros pasada al arrancar la aplicación, y configurar
  la instancia de App con una serie de opciones reconocidas (p.e., -pause arranca la aplicación
  en modo de pausa).
  Hay dos tipos de opciones:
  -las reservadas (-pause -size, etc.)
  -las de usuario

  Estas últimas son del tipo
  -o arg1 [{arg2 arg3... argn}]

  y empiezan con la opción -o, seguida de una cadena, que además puede tener
  opcionalmente una serie de argumentos entre llaves, p.e:

  -o anaglyph
  -o scale { 2 4 6 }

  Ejemplo de uso:

  CommandLineProcessor cmd(argc, argv);

  // Estaba la opción -o anaglyph en la línea de comandos?
  if (hasUserOption("anaglyph")) ...

  // Devolver el 3er argumento de la opción scale, p.e.:
  // -scale {2 4 6}
  // devolvería 6
  std::string s = getUserOptionParam("scale", 2);
  */
  class CommandLineProcessor
  {

  public:
    /**
    Este método se usa cuando la aplicación cliente no necesita tener acceso a la línea de
    comandos.
    */
    static bool process(int argc, char *argv[], App &instance);
    /**
    Crea un objeto CommandLineProcessor si necesitas tener acceso a la línea de comandos antes
    de iniciar la aplicación.
    */
    CommandLineProcessor(int argc, char *argv[]);

    /**
    Aplica las opciones al objeto App
    */
    bool process(App & instance);
    /**
    \return el nombre del ejecutable
    */
    std::string getExecutableName() const {
      return executableName;
    };
    /**
    \return true si en la línea de comandos había -o \c opt ...
    */
    bool hasUserOption(const std::string &opt) const;
    /**
     Devuelve el argumento en la posición indicada de una opción de usuario
     \param opt La opción de usuario buscada (la que está detrás de un -o)
     \param pos la posición del argumento dentro de las llaves (empieza en 0)
     \return el argumento en la posición pos del argumento de usuario \c opt
     */
    std::string getUserOptionParam(const std::string &opt, unsigned int pos) const;

    //! \return la lista de opciones de la línea de órdenes
    static std::string getHelp();

  private:
    std::list<std::string> args;
    std::string executableName;
  };

};

#endif
