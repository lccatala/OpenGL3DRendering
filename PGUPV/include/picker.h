#pragma once

#include "nodeVisitor.h"

#include <vector>
#include <memory>
#include <glm/mat4x4.hpp>

namespace PGUPV {
	class Model;
	class Mesh;

	/**
	\class Picker

	Esta clase permite implementar la operación de picking (seleccionar un objeto de la ventana haciendo
	clic directamente sobre su imagen).

	El picker es independiente de la estructura de la escena, ya que recibe un vector con los modelos que la 
	componen, junto con un identificador para cada modelo. Es responsabilidad del programador construir dicho
	vector. En el caso de usar un grafo de escena, se puede usar la clase \sa PickerNodeVisitor para construir 
	dicho vector.

	Ejemplo de uso:

	Picker::PickData pick{x, y, windowWidth, windowHeight};
	PickerNodeVisitor rsc{ Picker::adjustProjMatrix(pick, proj) * view};
	scene->getRoot()->accept(rsc);
	auto nodes = rsc.getResult();
    uint32_t id = picking.pick(pick, view, proj, nodes);
	if (id == 0) {
		// no se ha seleccionado nada
	} else {
		// se ha hecho clic sobre el modelo id
	}

	*/

	class Picker {
	public:
		/**
		Contiene información sobre el píxel sobre el que ha hecho clic el usuario
		*/
		struct PickData {
			uint32_t x, y; // coordenadas del pixel seleccionado (origen esquina *superior* izquierda)
			uint32_t width, height; // tamaño de la ventana en píxeles
		};

		/**
		Contiene la lista de modelos sobre los que se va a hacer picking
		*/
		struct ModelId {
			glm::mat4 modelMatrix; // matriz del modelo que lleva m al espacio del mundo
			Model* m;	// geometría a dibujar
			uint32_t id;  // identificador de este objeto. ¡Cuidado! No usar el id 0 (es el fondo)
		};
		Picker();
		~Picker();
		/**
		Calcula el objeto sobre el que ha hecho clic el usuario
		\param pick información sobre la posición del clic
		\param viewMatrix la matriz view original de la cámara
		\param projMatrix la matriz projection original de la cámara 
		\return el identificador del objeto seleccionado
		*/
		uint32_t pick(const PickData &pick, const glm::mat4 &viewMatrix, const glm::mat4& projMatrix, const std::vector<ModelId> &objects);
		/**
		Reduce el volumen de la cámara a un frustum asimétrico alrededor del píxel seleccionado. Se puede usar este 
		método para reducir el volumen de la vista al recorrer la escena para hacer frustum culling.
		\param pick Información del clic del usuario y del tamaño de la ventana
		\param projMatrix volumen de la vista de la cámara actual
		*/
		static glm::mat4 adjustProjMatrix(const PickData& pick, const glm::mat4& projMatrix);
	private:
		class PickerImpl;
		std::unique_ptr<PickerImpl> pimpl;
	};


	/**
	\class PickerNodeVisitor

	Implementa el recorrido del grafo de escena con frustum culling para recoger todos los Model
	que caen dentro de la cámara. Construye un vector que se puede usar con la clase Picker
	*/
	class PickerNodeVisitor : public NodeVisitor {
	public:
		PickerNodeVisitor(const glm::mat4& viewproj) : viewprojMatrix{ viewproj } {};
		~PickerNodeVisitor() {};
		void apply(Group& group) override {
			if (!group.isVisible() || !PGUPV::overlapsViewVolume(group.getBB(), viewprojMatrix * mats.getMatrix()))
				return;
			traverse(group);
		};
		void apply(Transform& transform) override {
			if (!transform.isVisible() || !PGUPV::overlapsViewVolume(transform.getBB(), viewprojMatrix * mats.getMatrix()))
				return;
			mats.pushMatrix();
			mats.multMatrix(transform.getTransform());
			traverse(transform);
			mats.popMatrix();
		};
		void apply(Geode& geode) override {
			rendernodes.emplace_back(Picker::ModelId{ mats.getMatrix(), &geode.getModel(), geode.getId() });
		};
		void reset() { mats.reset(); rendernodes.clear(); }
		const std::vector<Picker::ModelId>& getResult() const {
			return rendernodes;
		}
	private:
		MatrixStack mats;
		glm::mat4 viewprojMatrix;
		std::vector<Picker::ModelId> rendernodes;
	};
}