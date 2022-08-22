#ifndef _COMMANDLINEPROCESSOR_H 
#define _COMMANDLINEPROCESSOR_H 2013

#include <list>
#include <string>

namespace PGUPV {
  class App;

  /**
  \class CommandLineProcessor

  Se encarga de procesar la lista de par�metros pasada al arrancar la aplicaci�n, y configurar
  la instancia de App con una serie de opciones reconocidas (p.e., -pause arranca la aplicaci�n
  en modo de pausa).
  Hay dos tipos de opciones:
  -las reservadas (-pause -size, etc.)
  -las de usuario

  Estas �ltimas son del tipo
  -o arg1 [{arg2 arg3... argn}]

  y empiezan con la opci�n -o, seguida de una cadena, que adem�s puede tener
  opcionalmente una serie de argumentos entre llaves, p.e:

  -o anaglyph
  -o scale { 2 4 6 }

  Ejemplo de uso:

  CommandLineProcessor cmd(argc, argv);

  // Estaba la opci�n -o anaglyph en la l�nea de comandos?
  if (hasUserOption("anaglyph")) ...

  // Devolver el 3er argumento de la opci�n scale, p.e.:
  // -scale {2 4 6}
  // devolver�a 6
  std::string s = getUserOptionParam("scale", 2);
  */
  class CommandLineProcessor
  {

  public:
    /**
    Este m�todo se usa cuando la aplicaci�n cliente no necesita tener acceso a la l�nea de
    comandos.
    */
    static bool process(int argc, char *argv[], App &instance);
    /**
    Crea un objeto CommandLineProcessor si necesitas tener acceso a la l�nea de comandos antes
    de iniciar la aplicaci�n.
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
    \return true si en la l�nea de comandos hab�a -o \c opt ...
    */
    bool hasUserOption(const std::string &opt) const;
    /**
     Devuelve el argumento en la posici�n indicada de una opci�n de usuario
     \param opt La opci�n de usuario buscada (la que est� detr�s de un -o)
     \param pos la posici�n del argumento dentro de las llaves (empieza en 0)
     \return el argumento en la posici�n pos del argumento de usuario \c opt
     */
    std::string getUserOptionParam(const std::string &opt, unsigned int pos) const;

    //! \return la lista de opciones de la l�nea de �rdenes
    static std::string getHelp();

  private:
    std::list<std::string> args;
    std::string executableName;
  };

};

#endif
