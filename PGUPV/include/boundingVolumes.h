
#ifndef _BOUNDINGVOLUMES_H
#define _BOUNDINGVOLUMES_H 2013

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include <fstream>
#include <vector>                             // for vector
#include <float.h>                            // for FLT_MAX
#include "common.h"                           // for MAX, uint

namespace PGUPV {
  /*
  Clase que contiene una caja de inclusión alineada a los ejes (las coordenadas mínimas y máximas de los
  vértices de un modelo)
  */

  struct BoundingBox {
    /**
    Constructor por defecto. La caja de inclusión no es válida.
    */
    BoundingBox() :
      min(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)),
      max(glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
    {};
    /**
    Constructor mediante dos esquinas opuestas.
    Da igual las esquinas que se proporcionen. Se calculará automáticamente
    las esquinas mínima y máximas
    \param p primera esquina
    \param q esquina opuesta a q
    */
    BoundingBox(glm::vec3 p, glm::vec3 q);
    /**
    \return la dimensión máxima de la caja de inclusión en los ejes principales
    */
    float getMaxDimension() const {
      return glm::compMax(max - min);
    }
    /**
    \return la coordenada del centro de la caja
    */
    glm::vec3 getCenter() const { return (max + min) / 2.0f; }
    /**
    Coordenadas de la esquina inferior izquierda posterior y de la superior derecha anterior
    */
    glm::vec3 min, max;
    /**
    \return si la caja es válida
    */
    bool isValid() const { return min.x != FLT_MAX && max.x != -FLT_MAX; }
    /**
    Agranda la caja de inclusión para abarcar 'other'
    \param other el volumen de inclusión a abarcar
    */
    void grow(const BoundingBox &other);
    /**
    Invalida la caja de inclusión
    */
    void reset() {
      min = glm::vec3(FLT_MAX);
      max = glm::vec3(-FLT_MAX);
    }
    /**
    Aplica la transformación a la caja de inclusión
    */
    void transform(const glm::mat4 &xform);
    /**
    Devuelve las 8 esquinas del volumen de inclusión. Empieza en la cara superior 
    en sentido antihorario, y luego las correspondientes en sentido horario de la 
    cara inferior. Empieza por el vértice superior izquierdo anterior.
    */
    std::vector<glm::vec4> getVertices() const;
  };

  /*
  Clase que contiene un volumen de inclusión esférico (el centro y el radio del volumen en coordenadas del modelo)
  vértices de un modelo
  */
  struct BoundingSphere {
    BoundingSphere(const glm::vec3 &center = glm::vec3(0.0f, 0.0f, 0.0f), float radius = -1.0f) : 
      center(center), radius(radius) {};
    bool isValid() const { return radius >= 0.0f; }
    void reset() {
      center = glm::vec3(0.0f, 0.0f, 0.0f); radius = -1.0f;
    }
    void grow(const BoundingSphere &other);
    void transform(const glm::mat4 &xform);
    glm::vec3 center;
    float radius;
  };


  std::ostream &operator<<(std::ostream &os, const BoundingBox &s);
  std::ostream &operator<<(std::ostream &os, const BoundingSphere &s);

  BoundingBox computeBoundingBox(const float *v, uint ncomponents, size_t n);
  BoundingSphere computeBoundingSphere(const float *v, uint ncomponents, size_t n);

  //! \return true, si la caja de inclusión interseca con el volumen de la vista definido por mvp (producto
  //! de las matrices model, view y projection.
  bool overlapsViewVolume(const BoundingBox &bb, const glm::mat4& mvp);
};



#endif
