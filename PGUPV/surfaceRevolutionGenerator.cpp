#include <glm/gtc/matrix_transform.hpp>

#include "surfaceRevolutionGenerator.h"
#include "mesh.h"
#include "drawCommand.h"

using PGUPV::SurfaceRevolutionGenerator;
using PGUPV::Mesh;

SurfaceRevolutionGenerator::SurfaceRevolutionGenerator() :
  nSlices(0), genTexCoords(true), genTangs(true), genNorms(true), range(TWOPIf),
  texCoordMap([](glm::vec2 p) { return p; }),
  rotationAxis(glm::vec3(0.0f, 1.0f, 0.0f))
{
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setNumSlices(uint n) {
  nSlices = n;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setProfile(std::vector<glm::vec3> points) {
  profile = points;
  reset();
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setColorFunction(std::function<glm::vec4(glm::vec2)> colorFunc) {
  colorMap = colorFunc;
  return *this;
}


SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setTexCoordFunction(std::function<glm::vec2(glm::vec2)> texCoordFunc) {
  texCoordMap = texCoordFunc;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setRotationAxis(glm::vec3 rotAxis) {
  this->rotationAxis = rotAxis;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::genTexCoordinates(bool generate) {
  genTexCoords = generate;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::genTangents(bool generate) {
  genTangs = generate;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::genNormals(bool generate) {
  genNorms = generate;
  return *this;
}

SurfaceRevolutionGenerator &SurfaceRevolutionGenerator::setSweepRange(float radians) {
  range = radians;
  return *this;
}


void SurfaceRevolutionGenerator::addVertex(const glm::vec4 &pos, const glm::vec2 &tc, const glm::vec3 &nm, const glm::vec3 &tn) {
  vertices.push_back(pos);
  if (genNorms)
    normals.push_back(nm);
  if (genTangs)
    tangents.push_back(tn);
  if (genTexCoords)
    tex_coord.push_back(texCoordMap(tc));
  if (colorMap != nullptr)
    colors.push_back(colorMap(tc));
}

void SurfaceRevolutionGenerator::reset() {
  vertices.clear();
  normals.clear();
  tangents.clear();
  tex_coord.clear();
  colors.clear();
  indices.clear();
}

std::shared_ptr<Mesh> SurfaceRevolutionGenerator::build() {
  reset();

  // If there is only a vertex in the profile, build a circle (cap)
  if (profile.size() == 1) {
    return buildCircle();
  }

  auto mesh = std::make_shared<Mesh>();;

  if (profile.empty() || nSlices < 2) {
    return mesh;
  }

  std::vector<glm::vec3> faceNormals;
  glm::vec3 plusZ = glm::vec3(.0f, .0f, 1.0f);
  std::vector<float> textureCoordinatesByLength{ 0.0f };
  for (size_t i = 0; i < profile.size() - 1; i++) {
    glm::vec3 ab = profile[i + 1] - profile[i];
    textureCoordinatesByLength.push_back(glm::length(ab));
    faceNormals.push_back(glm::normalize(glm::cross(ab, plusZ)));
  }

  // Normalize textureCoordinatesByLength
  for (size_t i = 1; i < textureCoordinatesByLength.size(); i++) {
    textureCoordinatesByLength[i] += textureCoordinatesByLength[i - 1];
  }
  float total = textureCoordinatesByLength.back();

  for (auto &l : textureCoordinatesByLength)
    l /= total;

  glm::vec3 tangent = glm::vec3(0.f, 0.f, -1.0f);

  float delta = range / nSlices;
  for (uint i = 0; i < profile.size(); i++) {
    auto originalVertex = glm::vec4(profile[i], 1.0f);
    glm::vec3 normal;
    if (i > 0) normal += faceNormals[i - 1];
    if (i < faceNormals.size()) normal += faceNormals[i];
    if (i < faceNormals.size() - 1) normal += faceNormals[i + 1];
    normal = glm::normalize(normal);

    // The first point in each ring is the original
    addVertex(originalVertex, glm::vec2(0.0f, textureCoordinatesByLength[i]), normal, tangent);
    float angle = delta;
    for (uint j = 1; j <= nSlices; j++, angle += delta) {
      glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
      auto n4 = rot * glm::vec4(normal, 1.0f);
      auto tn4 = rot * glm::vec4(tangent, 1.0f);
      addVertex(rot * originalVertex, glm::vec2(float(j) / nSlices, textureCoordinatesByLength[i]), glm::vec3(n4), glm::vec3(tn4));
    }
    if (range == TWOPIf) {
      // The last vertex in the ring is the same as the first, but with a different TC. Let's make
      // sure it is exactly the same
      vertices[vertices.size() - 1] = originalVertex;
      normals[vertices.size() - 1] = normal;
      tangents[vertices.size() - 1] = tangent;
    }
  }

  for (uint i = 1; i < profile.size(); i++) {
    uint top = i * (nSlices + 1);
    uint bottom = (i - 1) * (nSlices + 1);
    for (uint j = 0; j <= nSlices; j++) {
      indices.push_back(top++);
      indices.push_back(bottom++);
    }
    // end of ring: restart
    indices.push_back(static_cast<GLuint>(-1));
  }
  if (!indices.empty()) {
    indices.pop_back(); // The last restart is not needed
  }

  if (genTexCoords) assert(vertices.size() == tex_coord.size());
  if (colorMap != nullptr) assert(vertices.size() == colors.size());

  mesh->addVertices(vertices);
  mesh->addNormals(normals);
  mesh->addTangents(tangents);
  mesh->addIndices(indices);
  if (genTexCoords) mesh->addTexCoord(0, tex_coord);
  if (colorMap != nullptr) mesh->addColors(colors);

  auto dc = new DrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
  dc->setRestartIndex(static_cast<GLuint>(-1));
  dc->setPrimitiveRestart(true);

  mesh->addDrawCommand(dc);
  return mesh;
}

std::shared_ptr<Mesh> SurfaceRevolutionGenerator::buildCircle() {
  assert(profile.size() == 1);

  auto mesh = std::make_shared<Mesh>();

  glm::vec4 original = glm::vec4(profile[0], 1.0f);
  // center of the fan
  glm::vec3 center = glm::dot(rotationAxis, glm::vec3(original)) * rotationAxis / glm::dot(rotationAxis, rotationAxis);
  glm::vec3 normal = -glm::normalize(rotationAxis);
  glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(original) - center, normal));
  addVertex(glm::vec4(center, 1.0f), glm::vec2(0.5f, 0.5f), normal, tangent);

  float delta = TWOPIf / nSlices;

  addVertex(original, glm::vec2(1.0f, 0.5f), normal, tangent);
  float rads = delta;
  for (uint j = 1; j < nSlices; j++, rads += delta) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rads, rotationAxis);
    auto tn4 = rot * glm::vec4(tangent, 1.0f);
    addVertex(rot * original, glm::vec2(cos(rads) / 2.0f + 0.5f, sin(rads) / 2.0f + 0.5f), normal, glm::vec3(tn4));
  }
  addVertex(original, glm::vec2(1.0f, 0.5f), normal, tangent);

  if (genTexCoords) assert(vertices.size() == tex_coord.size());
  if (colorMap != nullptr) assert(vertices.size() == colors.size());

  mesh->addVertices(vertices);
  if (genTexCoords) mesh->addTexCoord(0, tex_coord);
  if (colorMap != nullptr) mesh->addColors(colors);

  mesh->addDrawCommand(new DrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size())));
  return mesh;
}

