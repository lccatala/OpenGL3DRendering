#pragma once
// 2016

#include <memory>
#include <glm/glm.hpp>
#include "glMatrices.h"
#include "boundingVolumes.h"

namespace PGUPV
{

  /*
  Clase abstracta que define las funciones que debe implementar cualquier clase
  que se pueda dibujar.
  */

  class Renderable {
  public:
    virtual ~Renderable();
    // Función encargada de dibujar. 
    virtual void render() = 0;
    // Devuelve la caja de inclusión alineada a los ejes
    virtual BoundingBox getBB() = 0;
    // Devuelve la esfera de inclusión del modelo
    virtual BoundingSphere getBS() = 0;
    // Devuelve la dimensión máxima en X, Y, Z
    float maxDimension();
    // Devuelve la posición del centro de la caja de inclusión
    glm::vec3 center();
  };
}