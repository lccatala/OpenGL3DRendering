#pragma once

#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "common.h"

namespace PGUPV {

  /**
  \class SurfaceRevolutionGenerator
  Clase de ayuda que genera una malla (objeto Mesh) mediante la creación de una
  superficie por la rotación de una silueta 2D. Por defecto, la malla tendrá
  normales, tangentes y coordenadas de textura, pero se puede pedir al objeto
  que no las genere.

  Ejemplo de uso:
  \code
  SurfaceRevolutionGenerator sg;
  sg.setNumSlices(30).setProfile(std::vector<glm::vec3> {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.05f, 0.05f, 0.0f),
    glm::vec3(1.1f, 0.1f, 0.0f),
    glm::vec3(1.2f, 0.3f, 0.0f)}).
    setColorFunction([](glm::vec2 uv) { return glm::vec4(uv.x, uv.y, 0.0f, 1.0f);  });

    auto vase = sg.build();
  \endcode
  */
  class Mesh;

  class SurfaceRevolutionGenerator {
  public:

    SurfaceRevolutionGenerator();
    /**
    Establece el número de rotaciones que generarán el sólido (a más rotaciones, más
    detallado, pero también se generarán más triángulos)
    \param n visto desde el eje de rotación, el número de sectores que se generan
    */
    SurfaceRevolutionGenerator &setNumSlices(uint n);
    /**
    Establece la silueta que se usará para generar la superficie del sólido
    \param points vértices de la silueta
    \warning Para obtener un objeto cerrado asegúrate de definir el inicio y el final de 
    la silueta en el eje de rotación. Define los vértices de la silueta de abajo a 
    arriba.
    */
    SurfaceRevolutionGenerator &setProfile(std::vector<glm::vec3> points);
    /**
    Establece la longitud del arco que generará cada punto del perfil. Por defecto, 360º
    \param radians ángulo en radianes
    */
    SurfaceRevolutionGenerator &setSweepRange(float radians);
    /**
    Indica si se desea que la malla resultante tenga coordenadas de textura o no
    \param generate si true, los vértices de la malla tendrán coordenadas de textura
    */
    SurfaceRevolutionGenerator &genTexCoordinates(bool generate = true);
    /**
    Indica si se desea que la malla resultante tenga tangentes o no
    \param generate si true, los vértices de la malla tendrán tangentes
    */
    SurfaceRevolutionGenerator &genTangents(bool generate = true);
    /**
    Indica si se desea que la malla resultante tenga normales o no
    \param generate si true, los vértices de la malla tendrán normales
    */
    SurfaceRevolutionGenerator &genNormals(bool generate = true);
    /**
    Establece una función que asignará el color de cada vértice, en un espacio paramétrico.
    \param colorFunc un objeto función (por ejemplo, una lambda), que recibe las coordenadas
    del vértice en un espacio paramétrico, y devuelve su color. Ambas coordenadas están 
    definidas entre 0 y 1. La primera coordenada indica el ángulo de rotación del vértice, y
    la segunda su altura. Por lo tanto, el primer vértice de la silueta en la primera rotación
    tendrá las coordenadas 0, 0, y el último vértice de la silueta en la primera rotación tendrá 
    el 0, 1.
    el 
    */
    SurfaceRevolutionGenerator &setColorFunction(std::function<glm::vec4(glm::vec2)> colorFunc);
    /**
    Establece una función que asignará la coordenada de textura de cada vértice, en un espacio 
    paramétrico (ver \sa SurfaceRevolutionGenerator.setColorFunction)
    \param texCoordFunc un objeto función que devuelve las coordenadas de textura del vértice
    en el punto del espacio paramétrico definido por dos coordenadas (ver \sa 
    SurfaceRevolutionGenerator.setColorFunction)
    Por defecto, las coordenadas de textura coinciden con las coordenadas del espacio paramétrico 
    (es decir, equivale a llamar a setTexCooordFunction con [](glm::vec2 p) { return p; } )

    */
    SurfaceRevolutionGenerator &setTexCoordFunction(std::function<glm::vec2(glm::vec2)> texCoordFunc);
    /**
    Establece el eje de rotación del sólido
    \param rotationAxis el eje alrededor del cual se rotará la silueta
    */
    SurfaceRevolutionGenerator &setRotationAxis(glm::vec3 rotationAxis);
    /**
    Construye y devuelve la malla. Llama a este método para obtener la malla.
    */
    std::shared_ptr<Mesh> build();
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de vértices
    */
    const std::vector<glm::vec4> &getVertices() const {
      return vertices;
    };
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de vértices
    */
    const std::vector<glm::vec3> &getNormals() const {
      return normals;
    };
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de coordenadas
    de textura
    */
    const std::vector<glm::vec2> &getTexCoords() const {
      return tex_coord;
    };
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de colores
    por vértice
    */
    const std::vector<glm::vec4> &getColors() const {
      return colors;
    };
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de índices
    */
    const std::vector<GLuint> &getIndices() const {
      return indices;
    };
    /**
    Puedes llamar a esta función *después* de construir el modelo para obtener la lista de tangentes
    */
    const std::vector<glm::vec3> &getTangents() const {
      return tangents;
    }

  private:
    std::vector<glm::vec3> profile;
    uint nSlices;
    bool genTexCoords, genTangs, genNorms;
    float range;
    std::function<glm::vec4(glm::vec2)> colorMap;
    std::function<glm::vec2(glm::vec2)> texCoordMap;
    glm::vec3 rotationAxis;

    void addVertex(const glm::vec4 &pos, const glm::vec2 &tc, const glm::vec3 &nm, const glm::vec3 &tn);
    std::shared_ptr<Mesh> buildCircle();
    void reset();
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec2> tex_coord;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;
  };
};
