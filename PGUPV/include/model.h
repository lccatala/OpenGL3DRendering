#ifndef _MODEL_H
#define _MODEL_H 2011

#include <map>
#include <vector>
#include <functional>
#include <ostream>

#include "common.h"
#include "mesh.h"
#include "renderable.h"

namespace PGUPV {

//  class Node;

  /* Un objeto Model almacena uno o varios punteros a objetos de tipo Mesh.
  Normalmente contendrá un modelo sencillo, que puede estar compuesto por
  diferentes partes (cada parte en un Mesh), pero que comparten el mismo
  sistema de coordenadas */
  class Model : public Renderable {
  public:
    Model();
    // Dibuja el modelo
    void render() override;
    // Devuelve la caja de inclusión del modelo
    BoundingBox getBB() override;
    // Devuelve la esfera de inclusión del modelo
    BoundingSphere getBS() override;
    // Añade una malla al modelo
    void addMesh(std::shared_ptr<Mesh> mesh);
    // Devuelve el número de mallas 
    uint getNMeshes() const { return (uint)meshes.size(); };
	// Libera la memoria de las mallas del modelo
	void clearMeshes();
    // Devuelve una referencia a la malla i-ésima
    Mesh &getMesh(size_t i);
    /**
    Función de conveniencia para procesar todas las mallas del modelo
    \param op se invocará a la función op en cada una de las mallas del modelo
    */
    void accept(std::function<void(Mesh &)> op);
    /**
    \return una cadena con información sobre el modelo 
    */
    const std::string to_string() const;
  protected:
    std::vector<std::shared_ptr<Mesh>> meshes;
    BoundingBox bbox;
    BoundingSphere bsph;
  };

};

#endif
