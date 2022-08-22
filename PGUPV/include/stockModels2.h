#pragma once

#include <glm/glm.hpp>
#include "model.h"

/*

 Modelos de la biblioteca Par Shapes, disponible en: http://github.prideout.net/shapes

 */

// Color por defecto para todos los modelos a los que se les puede asignar un
// color
static const glm::vec4 DEFAULT_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


namespace PGUPV {
  /**
   \class Torus
   Toro centrado en el origen
   Define posiciones, normales, coordenadas de textura y un color.

   */

  class Torus : public Model {
  public:
    /**
     Constructor
     \param slices número de anillos
     \param sectors número de sectores (vértices en cada anillo)
     \param sectionRadius radio de la sección
     \param outerRadius radio exterior
     \param color color opcional para toda la figura
     */
    Torus(int slices, int sectors, float sectionRadius, float outerRadius, const glm::vec4 &color = DEFAULT_COLOR);
  };


  /**
   \class Tetrahedron

   Tetraedro apoyado y centrado en el origen. El modelo contiene las normales
   */
  class Tetrahedron : public Model {
  public:
    explicit Tetrahedron(const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Octohedron

   Octoedro centrando en el origen. El modelo contiene las normales
   */
  class Octahedron : public Model {
  public:
    explicit Octahedron(const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Dodecahedron

   Dodecaedro centrado en el origen. El modelo contiene las normales
   */
  class Dodecahedron : public Model {
  public:
    explicit Dodecahedron(const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Icosahedron

   Icosaedro centrado en el origen. El modelo contiene las normales
   */
  class Icosahedron : public Model {
  public:
    explicit Icosahedron(const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class Rock

   Modelo de una roca con normales
   */
  class Rock : public Model {
  public:
    Rock(int seed, int nsubdivisions, const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
   \class TrefoilKnot

   Modelo de un nudo de Trefoil
   */
  class TrefoilKnot : public Model {
  public:
    TrefoilKnot(int slices, int stacks, float radius, const glm::vec4 &color = DEFAULT_COLOR);
  };

  /**
  \class KleinBottle

  Modelo de una botella de Klein
  */
  class KleinBottle : public Model {
  public:
    KleinBottle(int slices, int stacks, const glm::vec4 &color = DEFAULT_COLOR);
  };

};