
#include "stockModels2.h"
#include "drawCommand.h"


// Como esto no es m�o, no me hago cargo...
#ifdef _WIN32
#pragma warning(push, 0)
#pragma warning(disable : 4996)  
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#define PAR_SHAPES_IMPLEMENTATION
#include "../librerias/par_shapes/par_shapes.h"
#if _WIN32
#pragma warning(pop)
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#include "mesh.h"

using PGUPV::Torus;
using PGUPV::Mesh;
using PGUPV::Tetrahedron;
using PGUPV::Octahedron;
using PGUPV::Dodecahedron;
using PGUPV::Icosahedron;
using PGUPV::Rock;
using PGUPV::TrefoilKnot;
using PGUPV::KleinBottle;

using PGUPV::Model;

static std::shared_ptr<Mesh> parShapesToMesh(const par_shapes_mesh *shape) {
  auto mesh = std::make_shared<Mesh>();;

  mesh->addVertices(shape->points, 3, shape->npoints);
  if (shape->normals != nullptr)
    mesh->addNormals(shape->normals, shape->npoints);
  if (shape->tcoords != nullptr)
    mesh->addTexCoord(0, shape->tcoords, shape->npoints);
  mesh->addIndices(shape->triangles, shape->ntriangles * 3);
  mesh->addDrawCommand(new PGUPV::DrawElements(GL_TRIANGLES, shape->ntriangles * 3, GL_UNSIGNED_SHORT, nullptr));
  return mesh;
}


Torus::Torus(int slices, int sectors, float sectionRadius, float outerRadius, const glm::vec4 &color) {

  par_shapes_mesh *t = par_shapes_create_torus(slices, sectors, sectionRadius);
  par_shapes_scale(t, outerRadius, outerRadius, outerRadius);

  auto m = parShapesToMesh(t);
  m->setColor(color);
  addMesh(m);

  par_shapes_free_mesh(t);
}

static void parShapesToModel(Model &model, par_shapes_mesh *shape, const glm::vec4 &color) {
  auto m = parShapesToMesh(shape);
  m->setColor(color);
  model.addMesh(m);
  par_shapes_free_mesh(shape);
}



static void parShapesToFacetedModel(Model &model, par_shapes_mesh *shape, const glm::vec4 &color) {
  // Compute the normals for lighting
  par_shapes_unweld(shape, true);
  par_shapes_compute_normals(shape);

  parShapesToModel(model, shape, color);
}

Tetrahedron::Tetrahedron(const glm::vec4 &color) {
  parShapesToFacetedModel(*this, par_shapes_create_tetrahedron(), color);
}

Octahedron::Octahedron(const glm::vec4 &color) {
  parShapesToFacetedModel(*this, par_shapes_create_octahedron(), color);
}

Dodecahedron::Dodecahedron(const glm::vec4 &color) {
  parShapesToFacetedModel(*this, par_shapes_create_dodecahedron(), color);
}

Icosahedron::Icosahedron(const glm::vec4 &color) {
  parShapesToFacetedModel(*this, par_shapes_create_icosahedron(), color);
}

Rock::Rock(int seed, int nsubdivisions, const glm::vec4 &color) {
  parShapesToModel(*this, par_shapes_create_rock(seed, nsubdivisions), color);
}

TrefoilKnot::TrefoilKnot(int slices, int stacks, float radius, const glm::vec4 &color) {
  parShapesToModel(*this, par_shapes_create_trefoil_knot(slices, stacks, radius), color);
}

KleinBottle::KleinBottle(int slices, int stacks, const glm::vec4 &color) {
  parShapesToModel(*this, par_shapes_create_klein_bottle(slices, stacks), color);
}