
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <gsl/gsl>

#include "stockModels.h"
#include "indexedBindingPoint.h"
#include "transform.h"
#include "geode.h"
#include "drawCommand.h"
#include "utils.h"

using namespace PGUPV;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

Axes::Axes(float length) {
	const std::vector<vec3> vertices
	{ vec3(0.0, 0.0, 0.0), vec3(length, 0.0, 0.0), vec3(0.0, 0.0, 0.0),
	vec3(0.0, length, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, length) };

	const std::vector<vec4> axesColors
	{ vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0),
	vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0) };

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	m->addColors(axesColors);
	m->addVertices(vertices);
	m->addDrawCommand(new PGUPV::DrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size())));
}

// Top face. Counterclockwise
#define P0 glm::vec3(-0.5f, 0.5f, 0.5f)
#define P1 glm::vec3(0.5f, 0.5f, 0.5f)
#define P2 glm::vec3(0.5f, 0.5f, -0.5f)
#define P3 glm::vec3(-0.5f, 0.5f, -0.5f)

// Bottom face. These points are below P0-P3

#define P4 glm::vec3(-0.5f, -0.5f, 0.5f)
#define P5 glm::vec3(0.5f, -0.5f, 0.5f)
#define P6 glm::vec3(0.5f, -0.5f, -0.5f)
#define P7 glm::vec3(-0.5f, -0.5f, -0.5f)



WireBox::WireBox(float size, const glm::vec4 &color) : WireBox(size, size, size, color) {};

WireBox::WireBox(float xsize, float ysize, float zsize, const glm::vec4 &color) :
	WireBox(BoundingBox(glm::vec3(-xsize / 2, -ysize / 2, -zsize / 2), glm::vec3(xsize / 2, ysize / 2, zsize / 2)), color) {};

WireBox::WireBox(const BoundingBox &bbox, const glm::vec4 &color) {
	static const GLushort wireBoxIndices[] = {
	  6, 5, 1, 0, 4, 5, 6, 2, 1, 2, 3, 0, 3, 7, 4, 7, 6, 2, 3, 7
	};

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	m->addIndices(wireBoxIndices, arraySize(wireBoxIndices));
	m->setColor(color);

	auto vertices = bbox.getVertices();
	std::vector<glm::vec3> v3;
	m->addVertices(vertices);
	m->addDrawCommand(new PGUPV::DrawElements(GL_LINE_STRIP, static_cast<GLsizei>(arraySize(wireBoxIndices)),
		GL_UNSIGNED_SHORT, (void *)0));
}

#define PLUS_X vec3(1.0, 0.0, 0.0)
#define MINUS_X vec3(-1.0, 0.0, 0.0)
#define PLUS_Y vec3(0.0, 1.0, 0.0)
#define MINUS_Y vec3(0.0, -1.0, 0.0)
#define PLUS_Z vec3(0.0, 0.0, 1.0)
#define MINUS_Z vec3(0.0, 0.0, -1.0)

#define BL vec2(0.0, 0.0)
#define BR vec2(1.0, 0.0)
#define TL vec2(0.0, 1.0)
#define TR vec2(1.0, 1.0)

Box::Box(float size, const glm::vec4 &color) : Box(size, size, size, color) {};

Box::Box(float xsize, float ysize, float zsize, const glm::vec4 &color) {
	std::vector<vec3> vertices
	{ P0, P1, P3, P2, P4, P5, P0, P1, P5, P6, P1, P2,
	P6, P7, P2, P3, P7, P4, P3, P0, P7, P6, P4, P5 };

	const std::vector<vec3> normals
	{
	  PLUS_Y, PLUS_Y, PLUS_Y, PLUS_Y, PLUS_Z, PLUS_Z, PLUS_Z, PLUS_Z,
	  PLUS_X, PLUS_X, PLUS_X, PLUS_X, MINUS_Z, MINUS_Z, MINUS_Z, MINUS_Z,
	  MINUS_X, MINUS_X, MINUS_X, MINUS_X, MINUS_Y, MINUS_Y, MINUS_Y, MINUS_Y,
	};

	const std::vector<vec2> texCoords
	{ BL, BR, TL, TR, BL, BR, TL, TR, BL, BR, TL, TR,
	BL, BR, TL, TR, BL, BR, TL, TR, BL, BR, TL, TR };

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	for (uint i = 0; i < vertices.size(); i++)
		vertices[i] = vertices[i] * glm::vec3(xsize, ysize, zsize);

	m->addNormals(normals);
	m->addVertices(vertices);
	m->addTexCoord(0, texCoords);
	m->setColor(color);
	GLint count[] = { 4, 4, 4, 4, 4, 4 };
	GLint first[] = { 0, 4, 8, 12, 16, 20 };
	m->addDrawCommand(
		new PGUPV::MultiDrawArrays(GL_TRIANGLE_STRIP, first, count, 6));
}

#undef P0
#undef P1
#undef P2
#undef P3
#undef P4
#undef P5
#undef P6
#undef P7


Rect::Rect(float width, float height, const glm::vec4 &color, uint nVertX,
	uint nVertY) {
	assert(nVertX >= 2 && nVertY >= 2);

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	if (nVertX == 2 && nVertY == 2) {
		const glm::vec3 vertices[] = {
		  vec3(-width / 2, -height / 2, 0.0f),
		  vec3(width / 2, -height / 2, 0.0f),
		  vec3(width / 2, height / 2, 0.0f),
		  vec3(-width / 2, height / 2, 0.0f) };
		const glm::vec3 normal = vec3(0.0f, 0.0f, 1.0f);
		const glm::vec2 texCoords[] = {
		  vec2(0.0f, 0.0f),
		  vec2(1.0f, 0.0f),
		  vec2(1.0f, 1.0f),
		  vec2(0.0f, 1.0f)
		};
		m->setNormal(normal);
		m->addVertices(vertices, sizeof(vertices) / sizeof(vertices[0]));
		m->addTexCoord(0, texCoords, sizeof(texCoords) / sizeof(texCoords[0]));
		m->setColor(color);
		m->addDrawCommand(
			new PGUPV::DrawArrays(GL_TRIANGLE_FAN, 0, sizeof(vertices) / sizeof(vertices[0])));
	}
	else {
		uint nTotalVertices = nVertX * nVertY;

		std::vector<vec3> vs(nTotalVertices);
		std::vector<vec3> ns(nTotalVertices);
		std::vector<vec2> ts(nTotalVertices);

		uint nStrips = nVertY - 1;
		uint nIndicesPerStrip = 2 * nVertX + 1; // +1 : primitive restart
		uint nTotalIndices = nStrips * nIndicesPerStrip;

		std::vector<uint> is(nTotalIndices);

		uint iv = 0;
		for (uint j = 0; j < nVertY; j++) {
			for (uint i = 0; i < nVertX; i++) {
				vs[iv] = vec3(i * width / (nVertX - 1) - width / 2,
					j * height / (nVertY - 1) - height / 2, 0.0f);
				ns[iv] = vec3(0.0, 0.0, 1.0);
				ts[iv] = vec2((float)i / (nVertX - 1), (float)j / (nVertY - 1));
				iv++;
			}
		}

		iv = 0;
		for (uint j = 0; j < nStrips; j++) {
			uint upperIndex = (j + 1) * nVertX;
			uint lowerIndex = j * nVertX;
			for (uint i = 0; i < nVertX; i++) {
				is[iv++] = upperIndex++;
				is[iv++] = lowerIndex++;
			}
			is[iv++] = (uint)-1;
		}

		m->addVertices(vs);
		m->setColor(color);
		m->addNormals(ns);
		m->addTexCoord(0, ts);
		m->addIndices(is);


		auto dc = new PGUPV::DrawElements(GL_TRIANGLE_STRIP, nTotalIndices,
			GL_UNSIGNED_INT, (void *)0);
		dc->setPrimitiveRestart();
		dc->setRestartIndex((uint)-1);
		m->addDrawCommand(dc);
	}
}

Disk::Disk(float r_in, float r_out, uint slices, uint rings, const glm::vec4 &color) {

	uint total_unique_vertices = slices * (rings + 1);
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> tex_coord;

	float delta = (float)(2.0 * M_PI / slices);
	uint k = 0;
	for (uint j = 0; j <= rings; j++) {
		float angle = 0.0;
		float r = r_out - j * (r_out - r_in) / rings;
		for (uint i = 0; i < slices; i++, angle += delta) {
			float cos_a = cos(angle);
			float sin_a = sin(angle);
			vertices.push_back(vec3(r * cos_a, r * sin_a, 0.0));
			normals.push_back(vec3(0.0f, 0.0f, 1.0f));
			tex_coord.push_back(vec2(vertices[k].x, vertices[k].y) / (2 * r_out) +
				vec2(0.5, 0.5));
			k++;
		}
	}

	assert(k == total_unique_vertices);

	uint total_indices = rings * (2 * (slices + 1)) + rings;
	std::vector<GLuint> indices;

	k = 0;
	for (uint j = 0; j < rings; j++) {
		uint left = slices * (j + 1);
		uint right = slices * j;
		for (uint i = 0; i < slices; i++) {
			indices.push_back(left++);
			indices.push_back(right++);
			k += 2;
		}
		indices.push_back(slices * (j + 1));
		indices.push_back(slices * j);
		indices.push_back(static_cast<GLuint>(-1));
		k += 3;
	}

	assert(k == total_indices);

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	m->addVertices(vertices);
	m->setColor(color);
	m->addNormals(normals);
	m->addTexCoord(0, tex_coord);
	m->addIndices(indices);

	auto dc = new DrawElements(GL_TRIANGLE_STRIP, total_indices,
		GL_UNSIGNED_INT, (void *)0);
	dc->setPrimitiveRestart();
	dc->setRestartIndex(static_cast<GLuint>(-1));
	m->addDrawCommand(dc);
}

std::shared_ptr<Arrow> Arrow::build(float length, float shaft_radio, float head_radio, float head_ratio,
	const glm::vec4 &head_color, const glm::vec4 &shaft_color) {
	auto theArrow = std::shared_ptr<Arrow>(new Arrow());
	float shaft_length = length * (1.0f - head_ratio);
	float head_length = head_ratio * length;

	glm::mat4 shaft_xf =
		glm::translate(glm::mat4(1.0f), vec3(0.0f, 0.0f, shaft_length / 2.0f));
	shaft_xf = glm::rotate(shaft_xf, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	auto shaftXform = Transform::build(shaft_xf);
	shaftXform->addChild(
		Geode::build(std::shared_ptr<Cylinder>(
			new Cylinder(shaft_radio, shaft_radio, shaft_length, 2, 20, shaft_color))));
	shaftXform->setName("Astil");
	glm::mat4 head_xf = glm::translate(
		glm::mat4(1.0f), vec3(0.0f, 0.0f, shaft_length + head_length / 2.0f));
	head_xf = glm::rotate(head_xf, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

	auto headXform = Transform::build(head_xf);
	headXform->addChild(
		Geode::build(std::shared_ptr<Cylinder>(
			new Cylinder(head_radio, 1E-4f, head_length, 10, 20, head_color))));
	headXform->setName("Punta");

	auto headbottomXform = Transform::build(glm::translate(glm::mat4(1.0f), vec3(0.0f, 0.0f, shaft_length)));
	headbottomXform->addChild(
		Geode::build(std::shared_ptr<Disk>(new Disk(0.0f, head_radio, 20, 4, head_color))));
	headbottomXform->setName("Base punta");

	auto bottom = Geode::build(std::shared_ptr<Disk>(
		new Disk(0.0f, shaft_radio, 20, 2, head_color)));
	bottom->setName("Base astil");

	theArrow->addChild(shaftXform);
	theArrow->addChild(headXform);
	theArrow->addChild(headbottomXform);
	theArrow->addChild(bottom);

	return theArrow;
}

Cylinder::Cylinder(float r_bottom, float r_top, float height, uint stacks,
	uint slices, const glm::vec4 &color) {

	if (r_bottom == 0.0 && r_top == 0.0)
		ERRT("Ambos radios no pueden ser cero.");
	if (r_bottom == 0.0 || r_top == 0.0)
		WARN("Para construir un cono, es preferible definir un radio muy pequeño, "
			"en vez de 0.0");

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	uint total_unique_vertices = (slices + 1) * (stacks + 1);
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> tex_coord;

	float tilt_angle = atan2(height, r_bottom - r_top) - glm::radians(90.0f);
	mat4 tilt_mat =
		glm::rotate(glm::mat4(1.0f), tilt_angle, vec3(1.0f, 0.0f, 0.0f));
	float delta = (float)(TWOPIf / slices);
	for (uint j = 0; j <= stacks; j++) {
		float angle = 0.0;
		float ring_y = height * ((float)j / stacks - 0.5f);
		float radius_ring = r_bottom - j * (r_bottom - r_top) / stacks;
		for (uint i = 0; i <= slices; i++, angle += delta) {
			float sin_a = sin(angle);
			float cos_a = cos(angle);

			vertices.push_back(
				vec3(radius_ring * sin_a, ring_y, radius_ring * cos_a));
			normals.push_back(
				vec3(glm::rotate(glm::mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f)) *
					tilt_mat * vec4(0.0f, 0.0f, 1.0f, 0.0f)));
			tex_coord.push_back(vec2(angle / TWOPIf, (float)j / stacks));
		}
	}

	assert(vertices.size() == total_unique_vertices);

	// # of indices per stack:
	//  each stack has 2*slices triangles
	//  a triangle strip requires 2 + #triangles = 2 + 2*slices
	//  we end each slice with a -1 index to ask OpenGL to start a new
	//  tri-strip (+1)
	uint indices_per_stack = 2 + 2 * slices + 1;

	std::vector<GLuint> indices;

	for (uint j = 0; j < stacks; j++) {
		uint topring = (j + 1) * (slices + 1);
		uint botring = j * (slices + 1);
		for (uint i = 0; i <= slices; i++) {
			indices.push_back(topring++);
			indices.push_back(botring++);
		}
		indices.push_back((uint)-1);
	}

	assert(indices_per_stack * stacks == indices.size());

	m->addIndices(indices);
	m->setColor(color);
	m->addVertices(vertices);
	m->addNormals(normals);
	m->addTexCoord(0, tex_coord);

	auto dc = new DrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_INT, (void *)0);
	dc->setPrimitiveRestart();
	dc->setRestartIndex((uint)-1);

	m->addDrawCommand(dc);
}

Sphere2::Sphere2(float radius, uint subdivision,
	const glm::vec4 &color) {
	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	// Number of vertices per parametric dimension
	uint nvs = subdivision + 2;
	uint nvt = nvs;

	ulong total_unique_vertices = nvs * nvt;

	std::vector<vec3> vertices(total_unique_vertices);
	std::vector<vec3> normals(total_unique_vertices);
	std::vector<vec2> tex_coord(total_unique_vertices);
	std::vector<vec3> tangents(total_unique_vertices);

	float deltas = 1.0f / (nvs - 1);
	float deltat = 1.0f / (nvt - 1);
	float t = 0.0f;
	uint index = 0;
	for (uint j = 0; j < nvt; j++) {
		float s = 0.0f;
		for (uint i = 0; i < nvs; i++) {
			vec3 p = vec3(1.0f - 2.0f * s, 1.0f - 2.0f * t, 1.0f);
			vec3 q = glm::normalize(p);

			normals[index] = q;
			tex_coord[index] = vec2(s, t);
			vertices[index] = vec3(q * radius);
			float dostm1 = 2.0f * t - 1.0f;
			float dossm1 = 2.0f * s - 1.0f;
			tangents[index] = glm::normalize(
				vec3(dostm1 * dostm1 + 1.0f, -dossm1 * dostm1, dossm1));
			s += deltas;
			index++;
		}
		t += deltat;
	}

	std::vector<GLuint> indices((nvt - 1) * (2 * nvs + 1));

	// Strips...
	index = 0;
	for (uint j = 0; j < nvt - 1; j++) {
		uint first = (j + 1) * nvs;
		for (uint i = 0; i < nvs; i++) {
			indices[index++] = first;
			indices[index++] = first - nvs;
			first++;
		}
		indices[index++] = (uint)-1;
	}

	m->addIndices(indices);
	m->setColor(color);
	m->addVertices(vertices);
	m->addNormals(normals);
	m->addTangents(tangents);
	m->addTexCoord(0, tex_coord);


	auto dc = new DrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_INT, (void *)0);
	dc->setPrimitiveRestart();
	dc->setRestartIndex((uint)-1);

	m->addDrawCommand(dc);
}

Sphere::Sphere(float radius, uint stacks, uint slices, const glm::vec4 &color) {
	if (stacks < 2)
		stacks = 2;

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	uint total_unique_vertices = (slices + 1) * (stacks + 1);
	std::vector<vec3> vertices(total_unique_vertices);
	std::vector<vec3> normals(total_unique_vertices);
	std::vector<vec2> tex_coord(total_unique_vertices);

	float deltalong = (float)(2.0 * M_PI / slices);
	float deltalat = (float)(M_PI / stacks);

	// Regular stacks
	uint k = 0;
	float latitude = (float)(-M_PI / 2.0);
	for (uint j = 0; j <= stacks; j++, latitude += deltalat) {
		float sinlat = sin(latitude);
		float coslat = cos(latitude);
		float longitude = 0.0;
		float ring_y = radius * sinlat;
		for (uint i = 0; i <= slices; i++, longitude += deltalong) {
			float sinlon = sin(longitude);
			float coslon = cos(longitude);

			vertices[k] =
				vec3(radius * sinlon * coslat, ring_y, radius * coslon * coslat);
			normals[k] = vec3(sinlon * coslat, sinlat, coslon * coslat);
			tex_coord[k] = vec2(longitude / (2 * M_PI), latitude / M_PI + 0.5);
			k++;
		}
	}

	// # of indices per stack:
	//  each stack has 2*slices triangles
	//  a triangle strip requires 2 + #triangles = 2 + 2*slices
	//  we end each slice with a -1 index to ask OpenGL to start a new
	//  tri-strip (+1)
	uint indices_per_stack = 2 + 2 * slices + 1;

	std::vector<GLuint> indices(indices_per_stack * stacks);

	k = 0;
	for (uint j = 0; j < stacks; j++) {
		uint topring = (j + 1) * (slices + 1);
		uint botring = j * (slices + 1);
		for (uint i = 0; i <= slices; i++) {
			indices[k++] = topring++;
			indices[k++] = botring++;
		}
		indices[k++] = (uint)-1;
	}

	m->addIndices(indices);
	m->setColor(color);
	m->addVertices(vertices);
	m->addNormals(normals);
	m->addTexCoord(0, tex_coord);


	auto dc = new DrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices.size()),
		GL_UNSIGNED_INT, (void *)0);
	dc->setPrimitiveRestart();
	dc->setRestartIndex((uint)-1);
	m->addDrawCommand(dc);
}

ScreenPolygon::ScreenPolygon(const glm::vec4 &color) {
	std::vector<glm::vec3> vertices
	{ vec3(-1.0, 1.0, 0.0f), vec3(-1.0, -1.0, 0.0f), vec3(1.0, 1.0, 0.0f),
	vec3(1.0, -1.0, 0.0f) };

	std::vector<glm::vec2> texCoords
	{ vec2(0.0f, 1.0f), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f) };

	std::vector<glm::vec4> colors(vertices.size());
	for (uint i = 0; i < vertices.size(); i++)
		colors[i] = color;

	auto m = std::make_shared<Mesh>();;
	addMesh(m);

	m->addVertices(vertices);
	m->addTexCoord(0, texCoords);
	m->addColors(colors);

	m->addDrawCommand(
		new PGUPV::DrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size())));
}

void ScreenPolygon::render() {
	auto mats = std::dynamic_pointer_cast<GLMatrices>(PGUPV::gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX));
	mats->pushMatrix(GLMatrices::PROJ_MATRIX);
	mats->pushMatrix(GLMatrices::VIEW_MATRIX);
	mats->setMatrix(GLMatrices::PROJ_MATRIX, glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f));
	mats->loadIdentity(GLMatrices::VIEW_MATRIX);

	Model::render();
	mats->popMatrix(GLMatrices::PROJ_MATRIX);
	mats->popMatrix(GLMatrices::VIEW_MATRIX);
}

TeapotPatches::TeapotPatches() {
	const uint NumTeapotVertices = 306;
	const uint NumTeapotPatches = 32;
	const uint NumTeapotVerticesPerPatch = 16; // 4x4 Bezier patches
	const uint NumTeapotIndices = NumTeapotVerticesPerPatch * NumTeapotPatches;

	static const GLfloat teapotVertices[NumTeapotVertices * 3] = {
	  1.4f, 2.4f, 0.f, 1.4f, 2.4f, -0.784f, 0.784f,
	  2.4f, -1.4f, 0.f, 2.4f, -1.4f, 1.3375f, 2.53125f,
	  0.f, 1.3375f, 2.53125f, -0.749f, 0.749f, 2.53125f, -1.3375f,
	  0.f, 2.53125f, -1.3375f, 1.4375f, 2.53125f, 0.f, 1.4375f,
	  2.53125f, -0.805f, 0.84f, 2.53125f, -1.4375f, 0.f, 2.53125f,
	  -1.4375f, 1.5f, 2.4f, 0.f, 1.5f, 2.4f, -0.84f,
	  0.84f, 2.4f, -1.5f, 0.f, 2.4f, -1.5f, -0.784f,
	  2.4f, -1.4f, -1.4f, 2.4f, -0.784f, -1.4f, 2.4f,
	  0.f, -0.749f, 2.53125f, -1.3375f, -1.3375f, 2.53125f, -0.749f,
	  -1.3375f, 2.53125f, 0.f, -0.805f, 2.53125f, -1.4375f, -1.4375f,
	  2.53125f, -0.805f, -1.4375f, 2.53125f, 0.f, -0.84f, 2.4f,
	  -1.5f, -1.5f, 2.4f, -0.84f, -1.5f, 2.4f, 0.f,
	  -1.4f, 2.4f, 0.784f, -0.784f, 2.4f, 1.4f, 0.f,
	  2.4f, 1.4f, -1.3375f, 2.53125f, 0.749f, -0.749f, 2.53125f,
	  1.3375f, 0.f, 2.53125f, 1.3375f, -1.4375f, 2.53125f, 0.805f,
	  -0.805f, 2.53125f, 1.4375f, 0.f, 2.53125f, 1.4375f, -1.5f,
	  2.4f, 0.84f, -0.84f, 2.4f, 1.5f, 0.f, 2.4f,
	  1.5f, 0.784f, 2.4f, 1.4f, 1.4f, 2.4f, 0.784f,
	  0.749f, 2.53125f, 1.3375f, 1.3375f, 2.53125f, 0.749f, 0.805f,
	  2.53125f, 1.4375f, 1.4375f, 2.53125f, 0.805f, 0.84f, 2.4f,
	  1.5f, 1.5f, 2.4f, 0.84f, 1.75f, 1.875f, 0.f,
	  1.75f, 1.875f, -0.98f, 0.98f, 1.875f, -1.75f, 0.f,
	  1.875f, -1.75f, 2.f, 1.35f, 0.f, 2.f, 1.35f,
	  -1.12f, 1.12f, 1.35f, -2.f, 0.f, 1.35f, -2.f,
	  2.f, 0.9f, 0.f, 2.f, 0.9f, -1.12f, 1.12f,
	  0.9f, -2.f, 0.f, 0.9f, -2.f, -0.98f, 1.875f,
	  -1.75f, -1.75f, 1.875f, -0.98f, -1.75f, 1.875f, 0.f,
	  -1.12f, 1.35f, -2.f, -2.f, 1.35f, -1.12f, -2.f,
	  1.35f, 0.f, -1.12f, 0.9f, -2.f, -2.f, 0.9f,
	  -1.12f, -2.f, 0.9f, 0.f, -1.75f, 1.875f, 0.98f,
	  -0.98f, 1.875f, 1.75f, 0.f, 1.875f, 1.75f, -2.f,
	  1.35f, 1.12f, -1.12f, 1.35f, 2.f, 0.f, 1.35f,
	  2.f, -2.f, 0.9f, 1.12f, -1.12f, 0.9f, 2.f,
	  0.f, 0.9f, 2.f, 0.98f, 1.875f, 1.75f, 1.75f,
	  1.875f, 0.98f, 1.12f, 1.35f, 2.f, 2.f, 1.35f,
	  1.12f, 1.12f, 0.9f, 2.f, 2.f, 0.9f, 1.12f,
	  2.f, 0.45f, 0.f, 2.f, 0.45f, -1.12f, 1.12f,
	  0.45f, -2.f, 0.f, 0.45f, -2.f, 1.5f, 0.225f,
	  0.f, 1.5f, 0.225f, -0.84f, 0.84f, 0.225f, -1.5f,
	  0.f, 0.225f, -1.5f, 1.5f, 0.15f, 0.f, 1.5f,
	  0.15f, -0.84f, 0.84f, 0.15f, -1.5f, 0.f, 0.15f,
	  -1.5f, -1.12f, 0.45f, -2.f, -2.f, 0.45f, -1.12f,
	  -2.f, 0.45f, 0.f, -0.84f, 0.225f, -1.5f, -1.5f,
	  0.225f, -0.84f, -1.5f, 0.225f, 0.f, -0.84f, 0.15f,
	  -1.5f, -1.5f, 0.15f, -0.84f, -1.5f, 0.15f, 0.f,
	  -2.f, 0.45f, 1.12f, -1.12f, 0.45f, 2.f, 0.f,
	  0.45f, 2.f, -1.5f, 0.225f, 0.84f, -0.84f, 0.225f,
	  1.5f, 0.f, 0.225f, 1.5f, -1.5f, 0.15f, 0.84f,
	  -0.84f, 0.15f, 1.5f, 0.f, 0.15f, 1.5f, 1.12f,
	  0.45f, 2.f, 2.f, 0.45f, 1.12f, 0.84f, 0.225f,
	  1.5f, 1.5f, 0.225f, 0.84f, 0.84f, 0.15f, 1.5f,
	  1.5f, 0.15f, 0.84f, -1.6f, 2.025f, 0.f, -1.6f,
	  2.025f, -0.3f, -1.5f, 2.25f, -0.3f, -1.5f, 2.25f,
	  0.f, -2.3f, 2.025f, 0.f, -2.3f, 2.025f, -0.3f,
	  -2.5f, 2.25f, -0.3f, -2.5f, 2.25f, 0.f, -2.7f,
	  2.025f, 0.f, -2.7f, 2.025f, -0.3f, -3.f, 2.25f,
	  -0.3f, -3.f, 2.25f, 0.f, -2.7f, 1.8f, 0.f,
	  -2.7f, 1.8f, -0.3f, -3.f, 1.8f, -0.3f, -3.f,
	  1.8f, 0.f, -1.5f, 2.25f, 0.3f, -1.6f, 2.025f,
	  0.3f, -2.5f, 2.25f, 0.3f, -2.3f, 2.025f, 0.3f,
	  -3.f, 2.25f, 0.3f, -2.7f, 2.025f, 0.3f, -3.f,
	  1.8f, 0.3f, -2.7f, 1.8f, 0.3f, -2.7f, 1.575f,
	  0.f, -2.7f, 1.575f, -0.3f, -3.f, 1.35f, -0.3f,
	  -3.f, 1.35f, 0.f, -2.5f, 1.125f, 0.f, -2.5f,
	  1.125f, -0.3f, -2.65f, 0.9375f, -0.3f, -2.65f, 0.9375f,
	  0.f, -2.f, 0.9f, -0.3f, -1.9f, 0.6f, -0.3f,
	  -1.9f, 0.6f, 0.f, -3.f, 1.35f, 0.3f, -2.7f,
	  1.575f, 0.3f, -2.65f, 0.9375f, 0.3f, -2.5f, 1.125f,
	  0.3f, -1.9f, 0.6f, 0.3f, -2.f, 0.9f, 0.3f,
	  1.7f, 1.425f, 0.f, 1.7f, 1.425f, -0.66f, 1.7f,
	  0.6f, -0.66f, 1.7f, 0.6f, 0.f, 2.6f, 1.425f,
	  0.f, 2.6f, 1.425f, -0.66f, 3.1f, 0.825f, -0.66f,
	  3.1f, 0.825f, 0.f, 2.3f, 2.1f, 0.f, 2.3f,
	  2.1f, -0.25f, 2.4f, 2.025f, -0.25f, 2.4f, 2.025f,
	  0.f, 2.7f, 2.4f, 0.f, 2.7f, 2.4f, -0.25f,
	  3.3f, 2.4f, -0.25f, 3.3f, 2.4f, 0.f, 1.7f,
	  0.6f, 0.66f, 1.7f, 1.425f, 0.66f, 3.1f, 0.825f,
	  0.66f, 2.6f, 1.425f, 0.66f, 2.4f, 2.025f, 0.25f,
	  2.3f, 2.1f, 0.25f, 3.3f, 2.4f, 0.25f, 2.7f,
	  2.4f, 0.25f, 2.8f, 2.475f, 0.f, 2.8f, 2.475f,
	  -0.25f, 3.525f, 2.49375f, -0.25f, 3.525f, 2.49375f, 0.f,
	  2.9f, 2.475f, 0.f, 2.9f, 2.475f, -0.15f, 3.45f,
	  2.5125f, -0.15f, 3.45f, 2.5125f, 0.f, 2.8f, 2.4f,
	  0.f, 2.8f, 2.4f, -0.15f, 3.2f, 2.4f, -0.15f,
	  3.2f, 2.4f, 0.f, 3.525f, 2.49375f, 0.25f, 2.8f,
	  2.475f, 0.25f, 3.45f, 2.5125f, 0.15f, 2.9f, 2.475f,
	  0.15f, 3.2f, 2.4f, 0.15f, 2.8f, 2.4f, 0.15f,
	  0.f, 3.15f, 0.f, 0.f, 3.15f, -0.002f, 0.002f,
	  3.15f, 0.f, 0.8f, 3.15f, 0.f, 0.8f, 3.15f,
	  -0.45f, 0.45f, 3.15f, -0.8f, 0.f, 3.15f, -0.8f,
	  0.f, 2.85f, 0.f, 0.2f, 2.7f, 0.f, 0.2f,
	  2.7f, -0.112f, 0.112f, 2.7f, -0.2f, 0.f, 2.7f,
	  -0.2f, 0.002f, 3.15f, 0.f, -0.45f, 3.15f, -0.8f,
	  -0.8f, 3.15f, -0.45f, -0.8f, 3.15f, 0.f, -0.112f,
	  2.7f, -0.2f, -0.2f, 2.7f, -0.112f, -0.2f, 2.7f,
	  0.f, 0.f, 3.15f, 0.002f, -0.8f, 3.15f, 0.45f,
	  -0.45f, 3.15f, 0.8f, 0.f, 3.15f, 0.8f, -0.2f,
	  2.7f, 0.112f, -0.112f, 2.7f, 0.2f, 0.f, 2.7f,
	  0.2f, 0.45f, 3.15f, 0.8f, 0.8f, 3.15f, 0.45f,
	  0.112f, 2.7f, 0.2f, 0.2f, 2.7f, 0.112f, 0.4f,
	  2.55f, 0.f, 0.4f, 2.55f, -0.224f, 0.224f, 2.55f,
	  -0.4f, 0.f, 2.55f, -0.4f, 1.3f, 2.55f, 0.f,
	  1.3f, 2.55f, -0.728f, 0.728f, 2.55f, -1.3f, 0.f,
	  2.55f, -1.3f, 1.3f, 2.4f, 0.f, 1.3f, 2.4f,
	  -0.728f, 0.728f, 2.4f, -1.3f, 0.f, 2.4f, -1.3f,
	  -0.224f, 2.55f, -0.4f, -0.4f, 2.55f, -0.224f, -0.4f,
	  2.55f, 0.f, -0.728f, 2.55f, -1.3f, -1.3f, 2.55f,
	  -0.728f, -1.3f, 2.55f, 0.f, -0.728f, 2.4f, -1.3f,
	  -1.3f, 2.4f, -0.728f, -1.3f, 2.4f, 0.f, -0.4f,
	  2.55f, 0.224f, -0.224f, 2.55f, 0.4f, 0.f, 2.55f,
	  0.4f, -1.3f, 2.55f, 0.728f, -0.728f, 2.55f, 1.3f,
	  0.f, 2.55f, 1.3f, -1.3f, 2.4f, 0.728f, -0.728f,
	  2.4f, 1.3f, 0.f, 2.4f, 1.3f, 0.224f, 2.55f,
	  0.4f, 0.4f, 2.55f, 0.224f, 0.728f, 2.55f, 1.3f,
	  1.3f, 2.55f, 0.728f, 0.728f, 2.4f, 1.3f, 1.3f,
	  2.4f, 0.728f, 0.f, 0.f, 0.f, 1.5f, 0.15f,
	  0.f, 1.5f, 0.15f, 0.84f, 0.84f, 0.15f, 1.5f,
	  0.f, 0.15f, 1.5f, 1.5f, 0.075f, 0.f, 1.5f,
	  0.075f, 0.84f, 0.84f, 0.075f, 1.5f, 0.f, 0.075f,
	  1.5f, 1.425f, 0.f, 0.f, 1.425f, 0.f, 0.798f,
	  0.798f, 0.f, 1.425f, 0.f, 0.f, 1.425f, -0.84f,
	  0.15f, 1.5f, -1.5f, 0.15f, 0.84f, -1.5f, 0.15f,
	  0.f, -0.84f, 0.075f, 1.5f, -1.5f, 0.075f, 0.84f,
	  -1.5f, 0.075f, 0.f, -0.798f, 0.f, 1.425f, -1.425f,
	  0.f, 0.798f, -1.425f, 0.f, 0.f, -1.5f, 0.15f,
	  -0.84f, -0.84f, 0.15f, -1.5f, 0.f, 0.15f, -1.5f,
	  -1.5f, 0.075f, -0.84f, -0.84f, 0.075f, -1.5f, 0.f,
	  0.075f, -1.5f, -1.425f, 0.f, -0.798f, -0.798f, 0.f,
	  -1.425f, 0.f, 0.f, -1.425f, 0.84f, 0.15f, -1.5f,
	  1.5f, 0.15f, -0.84f, 0.84f, 0.075f, -1.5f, 1.5f,
	  0.075f, -0.84f, 0.798f, 0.f, -1.425f, 1.425f, 0.f,
	  -0.798f };

	static const GLushort
		teapotIndices[NumTeapotPatches * NumTeapotVerticesPerPatch] = {
		// cuerpo
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
		14, 15, 3, 16, 17, 18, 7, 19, 20, 21, 11, 22, 23, 24,
		15, 25, 26, 27, 18, 28, 29, 30, 21, 31, 32, 33, 24, 34,
		35, 36, 27, 37, 38, 39, 30, 40, 41, 0, 33, 42, 43, 4,
		36, 44, 45, 8, 39, 46, 47, 12, 12, 13, 14, 15, 48, 49,
		50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 15, 25, 26, 27,
		51, 60, 61, 62, 55, 63, 64, 65, 59, 66, 67, 68, 27, 37,
		38, 39, 62, 69, 70, 71, 65, 72, 73, 74, 68, 75, 76, 77,
		39, 46, 47, 12, 71, 78, 79, 48, 74, 80, 81, 52, 77, 82,
		83, 56, 56, 57, 58, 59, 84, 85, 86, 87, 88, 89, 90, 91,
		92, 93, 94, 95, 59, 66, 67, 68, 87, 96, 97, 98, 91, 99,
		100, 101, 95, 102, 103, 104, 68, 75, 76, 77, 98, 105, 106, 107,
		101, 108, 109, 110, 104, 111, 112, 113, 77, 82, 83, 56, 107, 114,
		115, 84, 110, 116, 117, 88, 113, 118, 119, 92,

		// asa
		120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
		134, 135, 123, 136, 137, 120, 127, 138, 139, 124, 131, 140, 141, 128,
		135, 142, 143, 132, 132, 133, 134, 135, 144, 145, 146, 147, 148, 149,
		150, 151, 68, 152, 153, 154, 135, 142, 143, 132, 147, 155, 156, 144,
		151, 157, 158, 148, 154, 159, 160, 68,

		// boca
		161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
		175, 176, 164, 177, 178, 161, 168, 179, 180, 165, 172, 181, 182, 169,
		176, 183, 184, 173, 173, 174, 175, 176, 185, 186, 187, 188, 189, 190,
		191, 192, 193, 194, 195, 196, 176, 183, 184, 173, 188, 197, 198, 185,
		192, 199, 200, 189, 196, 201, 202, 193,

		// tapa
		203, 203, 203, 203, 206, 207, 208, 209, 210, 210, 210, 210, 211, 212,
		213, 214, 203, 203, 203, 203, 209, 216, 217, 218, 210, 210, 210, 210,
		214, 219, 220, 221, 203, 203, 203, 203, 218, 223, 224, 225, 210, 210,
		210, 210, 221, 226, 227, 228, 203, 203, 203, 203, 225, 229, 230, 206,
		210, 210, 210, 210, 228, 231, 232, 211, 211, 212, 213, 214, 233, 234,
		235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 214, 219, 220, 221,
		236, 245, 246, 247, 240, 248, 249, 250, 244, 251, 252, 253, 221, 226,
		227, 228, 247, 254, 255, 256, 250, 257, 258, 259, 253, 260, 261, 262,
		228, 231, 232, 211, 256, 263, 264, 233, 259, 265, 266, 237, 262, 267,
		268, 241,

		// fondo
		269, 269, 269, 269, 278, 279, 280, 281, 274, 275, 276, 277, 92, 119,
		118, 113, 269, 269, 269, 269, 281, 288, 289, 290, 277, 285, 286, 287,
		113, 112, 111, 104, 269, 269, 269, 269, 290, 297, 298, 299, 287, 294,
		295, 296, 104, 103, 102, 95, 269, 269, 269, 269, 299, 304, 305, 278,
		296, 302, 303, 274, 95, 94, 93, 92 };

	auto m = std::make_shared<Mesh>();;
	m->addVertices(teapotVertices, 3, NumTeapotVertices);
	m->addIndices(teapotIndices, NumTeapotIndices);

	DrawElements *d =
		new DrawElements(GL_PATCHES, NumTeapotIndices, GL_UNSIGNED_SHORT, 0);
	d->setVerticesPerPatch(NumTeapotVerticesPerPatch);
	m->addDrawCommand(d);

	addMesh(m);
}

std::shared_ptr<BoneWidget> BoneWidget::build()
{
	const glm::vec4 lightGray{ 0.8f, 0.8f, 0.8f, 1.0f };

	auto bone = std::shared_ptr<BoneWidget>(new BoneWidget());
	auto root = Transform::build(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	root->addChild(Geode::build(std::make_shared<Sphere>(0.05f, 7, 7, lightGray)));

	auto pyramid1 = Transform::build(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.1f, 0.2f))
	);
	root->addChild(pyramid1);

	auto pyramid = std::make_shared<Pyramid>(1.0f, 1.0f, false, lightGray);

	pyramid1->addChild(Geode::build(pyramid));

	auto pyramid2 = Transform::build(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.9f, 0.2f)));
	root->addChild(pyramid2);
	pyramid2->addChild(Geode::build(pyramid));

	bone->addChild(root);
	return bone;
}


#define P0 glm::vec3{-0.5f, 0.0f, 0.5f}
#define P1 glm::vec3{ 0.5f, 0.0f, 0.5f}
#define P2 glm::vec3{ 0.5f, 0.0f, -0.5f}
#define P3 glm::vec3{-0.5f, 0.0f, -0.5f}
#define P4 glm::vec3{ 0.0f, 1.0f, 0.0f}

Pyramid::Pyramid(float size, float height, bool base, const glm::vec4 & color) {
	std::vector<vec3> vertices{
	 P0, P1, P4,
	 P1, P2, P4,
	 P2, P3, P4,
	 P0, P4, P3,
	 P0, P3, P2,
	 P0, P2, P1 };

	auto m = std::make_shared<Mesh>();
	addMesh(m);

	unsigned int nVertices = gsl::narrow<unsigned int>(vertices.size());
	if (!base) 
		nVertices -= 6;

	for (uint i = 0; i < nVertices; i++)
		vertices[i] *= glm::vec3(size, height, size);

	std::vector<glm::vec3> normals;

	for (size_t i = 0; i < nVertices; i += 3) {
		glm::vec3 n = glm::cross(vertices[i + 1] - vertices[i], vertices[i + 2] - vertices[i]);
		normals.push_back(glm::normalize(n));
	}

	m->addNormals(normals);
	m->addVertices(vertices);
	m->setColor(color);
	m->addDrawCommand(new PGUPV::DrawArrays(GL_TRIANGLES, 0, nVertices));
}

#undef P0
#undef P1
#undef P2
#undef P3
#undef P4
