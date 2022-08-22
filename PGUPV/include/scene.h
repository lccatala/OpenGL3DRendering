
#ifndef _PGUPV_SCENE_H
#define _PGUPV_SCENE_H 2011

#include <memory>
#include <ostream>
#include <functional>

#include "renderable.h"

namespace PGUPV {

	class Node;
	class AnimationClip;
	class BaseMaterial;
	class Mesh;

	/* Una escena es un objeto compuesto por diferentes nodos. Cada nodo contien un modelo, que puede
	estar compuesto por varios Meshes, y diferentes modelos pueden compartir un mismo
	Mesh. P.e., un coche está compuesto por cinco modelos (chasis + cuatro ruedas),
	donde cada rueda apunta al mismo Mesh. Cada modelo llevará asociado una transformación.
	*/

	class Scene : public Renderable {
	public:
		Scene();
		~Scene() {};
		void setRoot(std::shared_ptr<Node> root);
		std::shared_ptr<Node> getRoot() { return sceneRoot; }
		void render() override;
		BoundingBox getBB() override;
		BoundingSphere getBS() override;
		/**
		Aplica la función pasada a todas las mallas de la escena.
		\param op Una función (o lambda) que recibe una malla para su procesamiento
		*/
		void processMeshes(std::function<void(Mesh &)> op);
		void release();
		void print(std::ostream &os);

		void update(unsigned int ms);

		//! Añade un material nuevo a la escena
		void addMaterial(std::shared_ptr<BaseMaterial> mat) {
			materials.push_back(mat);
		}

		size_t getNumMaterials() const {
			return materials.size();
		}

		//! Devuelve el material con el nombre indicado
		std::shared_ptr<BaseMaterial> getMaterial(const std::string &name);

		//! Devuelve el material n-ésimo
		std::shared_ptr<BaseMaterial> getMaterial(unsigned int n) {
			return materials[n];
		}

		//! Sustituye el material indicado
		void setMaterial(unsigned int n, std::shared_ptr<BaseMaterial> mat) {
			if (n >= materials.size()) {
				return;
			}
			materials[n] = mat;
		}

		/**
		Selecciona los nodos cuyo nombre contiene la cadena indicada (no tiene en cuenta
		  las mayúsculas/minúsculas. Los nodos que ya estaban seleccionados siguen seleccionados.
		  \param substr Cadena que debe contener el nombre de los nodos que se seleccionarán
		*/
		void selectNodesByName(const std::string &substr);

		/**
		Deselecciona todos los nodos seleccionados
		*/
		void unselectAll();
		/**
		Recorre la escena eliminando todos los nodos seleccionados
		*/
		void removeSelectedNodes();

		/**
		Añade una animación a la escena
		\param animation la animación
		*/
		void addAnimation(std::shared_ptr<AnimationClip> animation);

		/**
		\return el número de animaciones en la escena
		*/
		size_t getNumAnimations() const;
		std::shared_ptr<AnimationClip> getAnimation(size_t index) const;
	private:
		std::shared_ptr<Node> sceneRoot;
		std::vector<std::shared_ptr<BaseMaterial>> materials;
		std::vector<std::shared_ptr<AnimationClip>> animations;
	};
};
#endif
