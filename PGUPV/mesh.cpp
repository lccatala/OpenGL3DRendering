
#include <glm/glm.hpp>
#include <algorithm>
#include <sstream>
#include <gsl/gsl>

#include "indexedBindingPoint.h"
#include "bufferObject.h"
#include "mesh.h"
#include "utils.h"
#include "drawCommand.h"
#include "uboBones.h"
#include "skeleton.h"

using PGUPV::Mesh;
using PGUPV::BoundingBox;
using PGUPV::BaseMaterial;
using PGUPV::UniformBufferObject;
using PGUPV::BoundingSphere;
using PGUPV::DrawCommand;
using PGUPV::BufferObject;
using PGUPV::UBOBones;
using PGUPV::Skeleton;

using glm::vec2;
using glm::vec3;
using glm::vec4;

Mesh::Mesh() : vbos(_LAST_), epsilonSquared(1e-6f) {
	indices_type = 0;
	n_indices = 0;
	n_vertices = 0;
}

Mesh::~Mesh() { clearDrawCommands(); }

void Mesh::clearDrawCommands() {
	for (size_t i = 0; i < drawCommands.size(); i++) {
		SDELETE(drawCommands[i]);
	}
	drawCommands.clear();
};

// Devuelve la posición del centro de la caja de inclusión
glm::vec3 Mesh::center() const { return bb.max - bb.min; }

void Mesh::createBufferAndCopy(uint attribIndex, size_t size, GLenum usage,
	const void *data) {
	vbos[attribIndex] = BufferObject::build(size, usage);

	// If possible assign the object a label
	std::string label;
	switch (attribIndex) {
	case INDICES:
		label = "Indices";
		break;
	case VERTICES:
		label = "Vertices";
		break;
	case NORMALS:
		label = "Normales";
		break;
	case COLORS:
		label = "Colores";
		break;
	case TEX_COORD0:
		label = "Coord. de textura (0)";
		break;
	case TEX_COORD1:
		label = "Coord. de textura (1)";
		break;
	case TEX_COORD2:
		label = "Coord. de textura (2)";
		break;
	case TEX_COORD3:
		label = "Coord. de textura (3)";
		break;
	case TANGENTS:
		label = "Tangentes";
		break;
	case BONE_IDS:
		label = "Huesos (ids)";
		break;
	case BONE_WEIGHTS:
		label = "Huesos (pesos)";
		break;
	default:
		label = "Atributo Indeterminado";
	}
	vbos[attribIndex]->setGlDebugLabel(label);

	if (attribIndex == INDICES) {
		gl_element_array_buffer.bind(vbos[attribIndex]);
		gl_element_array_buffer.write(data);
	}
	else {
		gl_array_buffer.bind(vbos[attribIndex]);
		gl_array_buffer.write(data);
	}
}

void Mesh::addVertices(const glm::vec2 *v, size_t n, GLenum usage) {
	addVertices(&v[0].x, 2, n, usage);
}

void Mesh::addVertices(const glm::vec3 *v, size_t n, GLenum usage) {
	addVertices(&v[0].x, 3, n, usage);
}

void Mesh::addVertices(const glm::vec4 *v, size_t n, GLenum usage) {
	addVertices(&v[0].x, 4, n, usage);
}

void Mesh::addVertices(const float *v, uint ncomponents, size_t nVertices,
	GLenum usage) {

	prepareNewVBO(VERTICES);

	n_vertices = nVertices;
	if (nVertices == 0)
		return;
	n_components_per_vertex = ncomponents;

	createBufferAndCopy(VERTICES, sizeof(float) * n_components_per_vertex * n_vertices, usage, v);
	glEnableVertexAttribArray(VERTICES);
	glVertexAttribPointer(VERTICES, n_components_per_vertex, GL_FLOAT, GL_FALSE, 0, 0);

	bb = computeBoundingBox(v, ncomponents, nVertices);
	bs = computeBoundingSphere(v, ncomponents, nVertices);
}

static glm::vec3 getVec3FromMemory(float *cs, uint ncomponents, uint which) {
	ulong off = ncomponents * which;
	if (ncomponents == 3 || ncomponents == 4) return glm::vec3(cs[off], cs[off + 1], cs[off + 2]);
	else if (ncomponents == 2) return glm::vec3(cs[off], cs[off + 1], 0.0f);
	else return glm::vec3(cs[off], 0.0f, 0.0f);
}



bool Mesh::sameVertex(float *vs, uint ncomponents, uint a, uint b) {
	float distance = 0.0;
	for (uint i = 0; i < ncomponents; i++) {
		float t = vs[ncomponents*  a + i] - vs[ncomponents*  b + i];
		distance += t * t;
	}
	return distance < epsilonSquared;
}

void Mesh::computeSmoothNormals() {

	if (drawCommands.empty()) {
		ERRT("Llama a Mesh::computeSmoothNormals *después* de haber definido "
			"completamente la malla, con sus vértices, índices y drawCommands");
	}
	vao.bind();

	void *indices;
	if (n_indices == 0) {
		indices = nullptr;
	}
	else {
		auto idx = getBufferObject(INDICES);
		gl_copy_write_buffer.bind(idx);
		indices = gl_copy_write_buffer.map(GL_READ_ONLY);
	}

	// Conseguir los vértices
	auto vtx = getBufferObject(VERTICES);
	gl_copy_read_buffer.bind(vtx);
	GLfloat *vs = static_cast<GLfloat *>(gl_copy_read_buffer.map(GL_READ_ONLY));

	// Preguntar por su formato
	GLint ncomponents;
	GLint vtxFormat;
	glGetVertexAttribiv(VERTICES, GL_VERTEX_ATTRIB_ARRAY_SIZE, &ncomponents);
	glGetVertexAttribiv(VERTICES, GL_VERTEX_ATTRIB_ARRAY_TYPE, &vtxFormat);

	assert(vtxFormat == GL_FLOAT);

	std::vector<glm::vec3> smoothNormals(n_vertices, glm::vec3(0.0f, 0.0f, 0.0f));
	std::vector<uint> count(n_vertices, 0);

	// Para cada DrawCommand...
	for (auto drawCommand : drawCommands) {
		auto ids = drawCommand->getTrianglesIndices(indices);
		for (auto t : ids) {
			assert(t.idx[0] < n_vertices && t.idx[1] < n_vertices && t.idx[2] < n_vertices);
			auto a = getVec3FromMemory(vs, ncomponents, t.idx[0]);
			auto b = getVec3FromMemory(vs, ncomponents, t.idx[1]);
			auto c = getVec3FromMemory(vs, ncomponents, t.idx[2]);

			// Normal geométrica al triángulo
			glm::vec3 un = glm::cross(b - a, c - a);
			float module = glm::length(un);
			if (module < 1e-5) {
				// Triángulo degenerado: descartar
				continue;
			}

			auto n = un / module;

			// Acumulamos la normal a sus vértices, ponderada por el ángulo del vértice en cuestión
			float ang[3] = {
				acosf(glm::dot(glm::normalize(b - a), glm::normalize(c - a))),
				acosf(glm::dot(glm::normalize(c - b), glm::normalize(a - b))),
				acosf(glm::dot(glm::normalize(a - c), glm::normalize(b - c))) };

			for (int i = 0; i < 3; i++) {
				smoothNormals[t.idx[i]] += n * ang[i];
				count[t.idx[i]]++;
			}
		}
	}

	// Segunda vuelta para buscar vértices que geométricamente sean iguales, pero no semánticamente
	std::vector<GLuint> findDups(n_vertices);
	for (uint i = 0; i < n_vertices; i++) {
		findDups[i] = i;
	}

	std::sort(findDups.begin(), findDups.end(),
		[vs, ncomponents](GLuint a, GLuint b) {  // ordenar primero por la x, luego por la y, y luego por la z
		for (int i = 0; i < ncomponents; i++) {
			float va = vs[ncomponents * a + i];
			float vb = vs[ncomponents * b + i];
			if (va < vb) return true;
			if (va > vb) return false;
		}
		// son iguales: los dejamos como están
		return false;
	});

	// Ahora buscamos vértices que sean iguales (sus índices estarán correlativos en el vector findDups)
	uint firstInRun = 0;
	for (uint i = 1; i < findDups.size(); i++) {
		uint lastInRun = firstInRun;
		while (i < findDups.size() && sameVertex(vs, ncomponents, findDups[i - 1], findDups[i])) {
			lastInRun = i;
			i++;
		}
		if (lastInRun - firstInRun >= 1) {
			// Sumar todas las normales y asignarle el resultado a todos los vértices
			glm::vec3 normalSum(0.0f);
			for (uint j = firstInRun; j <= lastInRun; j++) {
				normalSum += smoothNormals[findDups[j]];
			}
			for (uint j = firstInRun; j <= lastInRun; j++) {
				smoothNormals[findDups[j]] = normalSum;
			}
		}
		firstInRun = i;
	}


	// Renormalizar
	for (uint i = 0; i < smoothNormals.size(); i++) {
		smoothNormals[i] = glm::normalize(smoothNormals[i]);
	}

	if (n_indices > 0) {
		gl_copy_write_buffer.unmap();
		gl_copy_write_buffer.unbind();
	}
	gl_copy_read_buffer.unmap();
	gl_copy_read_buffer.unbind();

	addNormals(smoothNormals);
}

void Mesh::addIndices(const GLubyte *i, size_t n, GLenum usage) {
	prepareNewVBO(INDICES);

	if (n == 0)
		return;

	createBufferAndCopy(INDICES, sizeof(GLubyte) * n, usage, i);
	indices_type = GL_UNSIGNED_BYTE;
	n_indices = n;
}

void Mesh::addIndices(const GLushort *i, size_t n, GLenum usage) {
	prepareNewVBO(INDICES);

	if (n == 0)
		return;

	createBufferAndCopy(INDICES, sizeof(GLushort) * n, usage, i);
	indices_type = GL_UNSIGNED_SHORT;
	n_indices = n;
}

void Mesh::addIndices(const GLuint *i, size_t n, GLenum usage) {
	prepareNewVBO(INDICES);

	if (n == 0)
		return;

	createBufferAndCopy(INDICES, sizeof(GLuint) * n, usage, i);
	indices_type = GL_UNSIGNED_INT;
	n_indices = n;
}

void Mesh::addNormals(const glm::vec3 *nr, size_t n, GLenum usage) {
	prepareNewVBO(NORMALS);

	if (n == 0)
		return;

	createBufferAndCopy(NORMALS, sizeof(glm::vec3) * n, usage, nr);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::addNormals(const GLfloat *nr, size_t n, GLenum usage) {
	prepareNewVBO(NORMALS);

	if (n == 0)
		return;

	createBufferAndCopy(NORMALS, sizeof(GLfloat) * n * 3, usage, nr);
	glEnableVertexAttribArray(NORMALS);
	glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::setNormal(const glm::vec3 &normal) {
	setAttribute(NORMALS, normal);
}

void Mesh::addTangents(const glm::vec3 *t, size_t n, GLenum usage) {
	prepareNewVBO(TANGENTS);

	if (n == 0)
		return;

	createBufferAndCopy(TANGENTS, sizeof(glm::vec3) * n, usage, t);
	glEnableVertexAttribArray(TANGENTS);
	glVertexAttribPointer(TANGENTS, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::addTangents(const float *t, size_t n, GLenum usage) {
	prepareNewVBO(TANGENTS);

	if (n == 0)
		return;

	createBufferAndCopy(TANGENTS, sizeof(GLfloat) * n * 3, usage, t);
	glEnableVertexAttribArray(TANGENTS);
	glVertexAttribPointer(TANGENTS, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::setTangent(const glm::vec3 &t) {
	setAttribute(TANGENTS, t);
}


void Mesh::setSkeleton(std::shared_ptr<Skeleton> skel)
{
	std::vector<glm::uvec4> boneIds;
	std::vector<glm::vec4> boneWeights;

	skel->buildArraysOfVerticesAndWeights(gsl::narrow<uint32_t>(n_vertices), boneIds, boneWeights);

	addBoneIds(boneIds);
	addBoneWeights(boneWeights);

	std::vector<glm::mat4> boneMatrices(skel->getNBones(), glm::mat4(1.0f));
	auto ub = UBOBones::build(boneMatrices);
	setBones(ub);

	skeleton = skel;
}

std::shared_ptr<Skeleton>Mesh::getSkeleton() const
{
	return skeleton;
}

void Mesh::addTexCoord(uint tex_unit, const glm::vec2 *t, size_t n,
	GLenum usage) {
	prepareNewVBO(TEX_COORD0 + tex_unit);

	if (n == 0)
		return;

	createBufferAndCopy(TEX_COORD0 + tex_unit, sizeof(glm::vec2) * n, usage, t);
	glEnableVertexAttribArray(TEX_COORD0 + tex_unit);
	glVertexAttribPointer(TEX_COORD0 + tex_unit, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::addTexCoord(uint tex_unit, const GLfloat *t, size_t n, GLenum usage) {
	prepareNewVBO(TEX_COORD0 + tex_unit);

	if (n == 0)
		return;

	createBufferAndCopy(TEX_COORD0 + tex_unit, sizeof(GLfloat) * 2 * n, usage, t);
	glEnableVertexAttribArray(TEX_COORD0 + tex_unit);
	glVertexAttribPointer(TEX_COORD0 + tex_unit, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::addColors(const glm::vec4 *c, size_t n, GLenum usage) {
	prepareNewVBO(COLORS);

	if (n == 0)
		return;

	createBufferAndCopy(COLORS, sizeof(glm::vec4) * n, usage, c);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::addColors(const float *c, uint ncomponents, size_t nColors,
	GLenum usage) {
	prepareNewVBO(COLORS);

	if (nColors == 0)
		return;

	createBufferAndCopy(COLORS, sizeof(float) * ncomponents * nColors, usage, c);
	glEnableVertexAttribArray(COLORS);
	glVertexAttribPointer(COLORS, ncomponents, GL_FLOAT, GL_FALSE, 0, 0);
}

void Mesh::setColor(const glm::vec4 &color) {
	setAttribute(COLORS, color);
}

void Mesh::addBoneIds(const std::vector<glm::uvec4> &boneIds, GLenum usage) {
	prepareNewVBO(BONE_IDS);

	if (boneIds.empty()) return;
	createBufferAndCopy(BONE_IDS, sizeof(glm::uvec4) * boneIds.size(), usage, &boneIds[0]);
	glEnableVertexAttribArray(BONE_IDS);
	glVertexAttribIPointer(BONE_IDS, 4, GL_UNSIGNED_INT, 0, 0);
}

void Mesh::addBoneWeights(const std::vector<glm::vec4> &boneWeights, GLenum usage) {
	prepareNewVBO(BONE_WEIGHTS);

	if (boneWeights.empty()) return;
	createBufferAndCopy(BONE_WEIGHTS, sizeof(glm::vec4) * boneWeights.size(), usage, &boneWeights[0]);
	glEnableVertexAttribArray(BONE_WEIGHTS);
	glVertexAttribPointer(BONE_WEIGHTS, 4, GL_FLOAT, 0, 0, 0);
}

void Mesh::addAttributeSetup(uint attribute_index, uint type_size,
	const void *a, uint ncomponents, size_t n, GLenum usage) {
	if (attribute_index < _LAST_)
		WARN("Cuidado! Ese índice puede usarse por la librería para almacenar atributos. Usa uno que no esté definido en mesh.h");
	GLint ma;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &ma);
	if (attribute_index >= (uint)ma)
		ERRT("Esta maquina no soporta tantos atributos");

	prepareNewVBO(attribute_index);

	if (n == 0)
		return;

	createBufferAndCopy(attribute_index, type_size * ncomponents * n, usage, a);
	glEnableVertexAttribArray(attribute_index);
}

void Mesh::addAttribute(uint attribute_index, GLenum type, uint type_size,
	const void *a, uint ncomponents, size_t n, GLenum usage) {

	addAttributeSetup(attribute_index, type_size, a, ncomponents, n, usage);
	glVertexAttribPointer(attribute_index, ncomponents, type, GL_FALSE, 0, 0);
}

void Mesh::addIntegerAttribute(uint attribute_index, GLenum type, uint type_size,
	const void *a, uint ncomponents, size_t n,
	GLenum usage)
{
#ifdef _DEBUG
	if (type != GL_BYTE && type != GL_UNSIGNED_BYTE && type != GL_SHORT && type != GL_UNSIGNED_SHORT
		&& type != GL_INT && type != GL_UNSIGNED_INT)
		ERRT("Este método sólo admite atributos de tipo entero");
#endif
	addAttributeSetup(attribute_index, type_size, a, ncomponents, n, usage);
	glVertexAttribIPointer(attribute_index, ncomponents, type, 0, 0);
}

void Mesh::setAttributeSetup(uint attribute_index, std::shared_ptr<BufferObject> bo) {
	GLint ma;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &ma);
	if (attribute_index >= (uint)ma)
		ERRT("Esta maquina no soporta tantos atributos");
	prepareNewVBO(attribute_index);

	vbos[attribute_index] = bo;
	gl_array_buffer.bind(vbos[attribute_index]);

	glEnableVertexAttribArray(attribute_index);
}

void Mesh::setAttribute(uint attribute_index, std::shared_ptr<BufferObject> bo,
	GLenum type, uint ncomponents) {
	setAttributeSetup(attribute_index, bo);
	glVertexAttribPointer(attribute_index, ncomponents, type, GL_FALSE, 0, 0);
}


void Mesh::setIntegerAttribute(uint attribute_index, std::shared_ptr<BufferObject> bo,
	GLenum type, uint ncomponents) {
#ifdef _DEBUG
	if (type != GL_BYTE && type != GL_UNSIGNED_BYTE && type != GL_SHORT && type != GL_UNSIGNED_SHORT
		&& type != GL_INT && type != GL_UNSIGNED_INT)
		ERRT("Este método sólo admite atributos de tipo entero");
#endif
	setAttributeSetup(attribute_index, bo);
	glVertexAttribIPointer(attribute_index, ncomponents, type, 0, 0);
}
// Establece un valor estático para el atributo de índice indicado. Todos los
// vértices compartirán dicho valor para el atributo
void Mesh::setAttribute(uint attribute_index, const glm::vec3 &a) {
	setAttribute(attribute_index, glm::vec4(a.x, a.y, a.z, 1.0));
}

// Establece un valor estático para el atributo de índice indicado. Todos los
// vértices compartirán dicho valor para el atributo
void Mesh::setAttribute(uint attribute_index, const glm::vec4 &a) {
	GLint ma;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &ma);
	if (attribute_index >= (uint)ma)
		ERRT("Esta maquina no soporta tantos atributos");

	if (attribute_index < vbos.size() && vbos[attribute_index]) {
		// Había un buffer previamente configurado?
		vbos[attribute_index].reset();
		WARN("Sobreescribiendo el buffer del atributo " +
			std::to_string(attribute_index) + " con un valor estático");
		vao.bind();
		glDisableVertexAttribArray(attribute_index);
	}
	addStaticAttributeValue(attribute_index, a);
}

void Mesh::addStaticAttributeValue(GLint index, const glm::vec4 &val) {
	for (uint i = 0; i < staticAttrValues.size(); i++) {
		if (staticAttrValues[i].attrIndex == index) {
			staticAttrValues[i].value = val;
			return;
		}
	}
	staticAttrValues.push_back(StaticAttribute(index, val));
}

bool Mesh::removeStaticAttributeValue(GLint index) {
	for (std::vector<StaticAttribute>::iterator i = staticAttrValues.begin();
		i != staticAttrValues.end(); ++i) {
		if (i->attrIndex == index) {
			staticAttrValues.erase(i);
			return true;
		}
	}
	return false;
}

void Mesh::render() {
	vao.bind();
	if (material) material->use();
	if (bones) bones->use();

	for (std::vector<StaticAttribute>::iterator i = staticAttrValues.begin();
		i != staticAttrValues.end(); ++i)
		glVertexAttrib4fv(i->attrIndex, &i->value.x);

#ifdef _DEBUG
	if (drawCommands.empty()) {
		WARN("Intentando dibujar un Mesh sin comandos de dibujo (no se dibujará nada)");
	}
#endif
	for (auto d : drawCommands)
		d->render();
	CHECK_GL();
}

size_t Mesh::getNNormals() const
{
	if (vbos[NORMALS])
		return n_vertices;
	return 0;
}

size_t Mesh::getNTexCoord(int texUnit) const
{
	if (vbos[TEX_COORD0 + texUnit])
		return n_vertices;
	return 0;
}

void Mesh::setMaterial(std::shared_ptr<BaseMaterial> m) { material = m; }

std::shared_ptr<BaseMaterial> Mesh::getMaterial() const {
	return material;
}

void Mesh::setBones(std::shared_ptr<UBOBones> b)
{
	bones = b;
}

std::shared_ptr<UBOBones> Mesh::getBones() const
{
	return bones;
}


std::ostream &PGUPV::operator<<(std::ostream &os, const BoundingBox &bb) {
	if (!bb.isValid()) {
		os << "[ EMPTY ]";
	}
	else {
		os << "[" << bb.min.x << " -> " << bb.max.x;
		os << ", " << bb.min.y << " -> " << bb.max.y;
		os << ", " << bb.min.z << " -> " << bb.max.z << "]";
	}
	return os;
}

std::ostream &PGUPV::operator<<(std::ostream &os, const BoundingSphere &s) {
	os << "BS: centro(" << s.center.x << ", " << s.center.y << ", " << s.center.z
		<< ")" << std::endl;
	os << "\t"
		<< "radio: " << s.radius << std::endl;
	return os;
}

void Mesh::prepareNewVBO(uint attribIndex) {
	vao.bind();
	if (attribIndex >= vbos.size()) {
		vbos.resize(attribIndex + 1);
	}
	if (removeStaticAttributeValue(attribIndex))
		WARN("Sustituyendo un valor estático asociado al atributo " +
			std::to_string(attribIndex) +
			" establecido previamente por un buffer");
}


void Mesh::addDrawCommand(DrawCommand *d) { drawCommands.push_back(d); }

std::shared_ptr<BufferObject> Mesh::getBufferObject(BufferObjectType which) {
	return vbos[which];
}


std::shared_ptr<BufferObject> Mesh::getBufferObject(int attribute) {
	return vbos[attribute];
}

void Mesh::setName(const std::string & n) {
	name = n;
}

const std::string &Mesh::getName() const {
	return name;
}

const std::string Mesh::to_string() const {
	std::ostringstream os;
	os << n_vertices << " vert " << drawCommands.size() << " draws";
	return os.str();
}

template <typename V>
std::vector<V> copyFromPFloatToVectorVec(std::shared_ptr<BufferObject> vbo, size_t count, int n_components = 3) {
	auto prev = PGUPV::gl_copy_read_buffer.bind(vbo);
	float *vb = static_cast<float *>(PGUPV::gl_copy_read_buffer.map(GL_READ_ONLY));
	assert(vb != nullptr);

	std::vector<V> dst;
	for (size_t i = 0; i < count; i++) {
		V v;
		for (typename V::length_type c = 0; c < V::length(); c++) {
			if (c < n_components) {
				v[c] = vb[c];
			}
			else {
				v[c] = 0.0f;
			}
		}
		dst.push_back(v);
		vb += n_components;
	}
	PGUPV::gl_copy_read_buffer.unmap();
	PGUPV::gl_copy_read_buffer.bind(prev);
	return dst;
}


std::vector<glm::vec3> Mesh::getVertices() const {
	auto dst = copyFromPFloatToVectorVec<glm::vec3>(vbos[VERTICES], n_vertices, n_components_per_vertex);
	return dst;
}

std::vector<glm::vec3> Mesh::getNormals() const {
	if (!vbos[NORMALS]) return std::vector<glm::vec3>();
	return copyFromPFloatToVectorVec<glm::vec3>(vbos[NORMALS], n_vertices);
}

std::vector<glm::vec2> Mesh::getTexCoords(unsigned int texCoordSet) const {
	if (!vbos[TEX_COORD0 + texCoordSet]) return std::vector<glm::vec2>();
	return copyFromPFloatToVectorVec<glm::vec2>(vbos[TEX_COORD0 + texCoordSet], n_vertices, 2);
}

template <typename T>
std::vector<unsigned int> fromPToTToVectorUint(const T *data, size_t count) {
	std::vector<unsigned int> res;
	for (size_t i = 0; i < count; i++) {
		res.push_back(data[i]);
	}
	return res;
}

std::vector<unsigned int> Mesh::getIndices() const
{
	std::vector<unsigned int> dst;
	auto prev = PGUPV::gl_copy_read_buffer.bind(vbos[INDICES]);

	switch (indices_type) {
	case GL_UNSIGNED_BYTE:
	{
		GLubyte *ids = static_cast<GLubyte *>(PGUPV::gl_copy_read_buffer.map(GL_READ_ONLY));
		assert(ids != nullptr);
		dst = fromPToTToVectorUint(ids, n_indices);
	}
	break;
	case GL_UNSIGNED_SHORT:
	{
		GLushort *ids = static_cast<GLushort *>(PGUPV::gl_copy_read_buffer.map(GL_READ_ONLY));
		assert(ids != nullptr);
		dst = fromPToTToVectorUint(ids, n_indices);
	}
	break;
	case GL_UNSIGNED_INT:
	{
		GLuint *ids = static_cast<GLuint *>(PGUPV::gl_copy_read_buffer.map(GL_READ_ONLY));
		assert(ids != nullptr);
		dst = fromPToTToVectorUint(ids, n_indices);
	}
	break;
	}
	PGUPV::gl_copy_read_buffer.unmap();
	PGUPV::gl_copy_read_buffer.bind(prev);
	return dst;
}
