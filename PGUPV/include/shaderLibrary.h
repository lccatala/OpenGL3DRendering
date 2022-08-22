#ifndef _SHADER_LIBRARY_H
#define _SHADER_LIBRARY_H 2013

#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "common.h"

namespace PGUPV {
  class Program;
  class ShaderLibrary {
  public:
    ShaderLibrary();
    void add(Program *shader);
    void remove(Program *shader);
    // Returns the number of registered programs
    uint size();
    // Returns the ith shader
    Program *get(uint i);
    /**
    Imprimir información sobre OpenGL y sobre todos los shaders instalados
    \param os flujo de salida donde escribir la información
    \param verbose si true, imprime más información, como por ejemplo la lista de extensiones
    */
    void printInfoShaders(std::ostream &os = std::cout, bool verbose = false);
  private:
    std::vector<Program *> library;
  };
};

#endif