#include <sstream>
#include <glm/glm.hpp>

#include "model.h"
#include "log.h"


using PGUPV::Model;
using PGUPV::Mesh;
using PGUPV::BoundingBox;
using PGUPV::BoundingSphere;
using PGUPV::GLMatrices;
using PGUPV::Renderable;


Model::Model() {
}

void Model::addMesh(std::shared_ptr<Mesh> mesh) {
	meshes.push_back(mesh);
	bbox.reset();
	bsph.reset();
}

void Model::clearMeshes() {
	meshes.clear();
	bbox.reset();
	bsph.reset();
}


Mesh &Model::getMesh(size_t i) {
	if (i >= meshes.size())
		ERRT("Esa malla no existe");
	return *(meshes[i]);
}

void Model::accept(std::function<void(Mesh&)> op) {
	for (auto m : meshes) {
		op(*m);
	}
}

const std::string Model::to_string() const {
	std::ostringstream os;
	os << meshes.size() << " mallas {";
	for (size_t i = 0; i < meshes.size(); i++) {
		os << " [" << i << "] " << meshes[i]->to_string();
	}
	os << "}";
	return os.str();
}

BoundingBox Model::getBB() {
	if (!bbox.isValid()) {
		for (const auto &m : meshes) {
			bbox.grow(m->getBB());
		}
	}
	return bbox;
}


BoundingSphere Model::getBS() {
	if (!bsph.isValid()) {
		for (const auto &m : meshes) {
			bsph.grow(m->getBS());
		}
	}
	return bsph;
}

void Model::render() {
	for (uint i = 0; i < meshes.size(); i++)
		meshes[i]->render();
}

