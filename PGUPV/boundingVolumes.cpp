#include <assert.h>
#include <cmath>                                      // for sqrt
#include <vector>                                     // for vector
#include <glm/glm.hpp>

#include "log.h"
#include "boundingVolumes.h"
#include "common.h"                                   // for MAX, uint, MIN
#include "utils.h"                                    // for distSquare

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

using PGUPV::BoundingBox;
using PGUPV::BoundingSphere;
using glm::vec3;

BoundingBox PGUPV::computeBoundingBox(const float *v, uint ncomponents, size_t n) {
	BoundingBox bb;

	uint zoffset = 2;
	if (ncomponents == 2) {
		zoffset = 1;
	}
	bb.min.x = bb.max.x = v[0];
	bb.min.y = bb.max.y = v[1];
	bb.min.z = bb.max.z = v[zoffset];
	for (uint i = ncomponents, j = 1; j < n; j++, i += ncomponents) {
		bb.min.x = MIN(bb.min.x, v[i]);
		bb.min.y = MIN(bb.min.y, v[i + 1]);
		bb.min.z = MIN(bb.min.z, v[i + zoffset]);

		bb.max.x = MAX(bb.max.x, v[i]);
		bb.max.y = MAX(bb.max.y, v[i + 1]);
		bb.max.z = MAX(bb.max.z, v[i + zoffset]);
	}

	if (ncomponents == 2) {
		bb.min.z = bb.max.z = 0.0;
	}
	return bb;
}

inline glm::vec3 floatArrayToVec3(const float *v, uint ncomponents) {
	if (ncomponents == 2)
		return glm::vec3(v[0], v[1], 0.0);
	else if (ncomponents == 3 || ncomponents == 4)
		return glm::vec3(v[0], v[1], v[2]);
	else
		ERRT("Sólo se aceptan vértices de 2 y 3 coordenadas");
}

BoundingSphere PGUPV::computeBoundingSphere(const float *v, uint ncomponents,
	size_t n) {
	// Jack Ritter. Graphics Gems, Academic Press 1990
	BoundingSphere sphere;

	auto bb = computeBoundingBox(v, ncomponents, n);
	sphere.center = bb.getCenter();
	sphere.radius = bb.getMaxDimension() / 2.0f;
	const float *vs = v;
	float rsq = sphere.radius * sphere.radius;
	for (uint i = 0; i < n; i++) {
		vec3 newp = floatArrayToVec3(vs, ncomponents);
		float dtovsq = distSquare(sphere.center, newp);
		if (dtovsq > rsq) {
			float dtov = sqrt(dtovsq);
			sphere.radius = (sphere.radius + dtov) / 2.0f;
			rsq = sphere.radius * sphere.radius;
			sphere.center =
				(sphere.radius * sphere.center + (dtov - sphere.radius) * newp) /
				dtov;
		}
		vs += ncomponents;
	}
	return sphere;
}

glm::bvec3 and3(const glm::bvec3& a, const glm::bvec3& b) {
	return glm::bvec3{
		a.x && b.x,
		a.y && b.y,
		a.z && b.z
	};
}

bool PGUPV::overlapsViewVolume(const BoundingBox& bb, const glm::mat4& mvp)
{
	if (!bb.isValid())
		return false;

	auto vtcs = bb.getVertices();
	glm::bvec3 lt{ true }, gt{ true };
	size_t i;
	for (i = 0; i < vtcs.size(); i++) {
		auto v4 = mvp * vtcs[i];
		auto v3 = glm::vec3(v4 / v4.w);
		lt = and3(lt, glm::lessThan(v3, glm::vec3(-v4.w)));
		gt = and3(gt, glm::greaterThan(v3, glm::vec3(v4.w)));
		if (!(glm::any(lt) || glm::any(gt)))
			return true;
	}
	return false;
}

BoundingBox::BoundingBox(glm::vec3 p, glm::vec3 q) {
	min = glm::min(p, q);
	max = glm::max(p, q);
}

void BoundingBox::grow(const BoundingBox & other) {
	if (other.isValid()) {
		min = glm::min(min, other.min);
		max = glm::max(max, other.max);
	}
}

void BoundingBox::transform(const glm::mat4 & xform) {
	if (!isValid())
		return;

	auto corners = getVertices();
	reset();
	for (auto &vtx : corners) {
		auto vp = xform * vtx;
		if (vp.w) vp = vp / vp.w;
		auto tmp = glm::vec3(vp);
		min = glm::min(min, tmp);
		max = glm::max(max, tmp);
	}
}

std::vector<glm::vec4> BoundingBox::getVertices() const {
	return std::vector<glm::vec4> {
		// Upper face
		glm::vec4(min.x, max.y, max.z, 1.0),
			glm::vec4(max, 1.0f),
			glm::vec4(max.x, max.y, min.z, 1.0),
			glm::vec4(min.x, max.y, min.z, 1.0),
			// Lower face
			glm::vec4(min.x, min.y, max.z, 1.0),
			glm::vec4(max.x, min.y, max.z, 1.0),
			glm::vec4(max.x, min.y, min.z, 1.0),
			glm::vec4(min, 1.0f)
	};
}

void BoundingSphere::grow(const BoundingSphere & other) {
	if (!other.isValid())
		return;
	if (!isValid()) {
		*this = other;
		return;
	}
	const float dist = glm::distance(center, other.center);
	const float r1 = radius;
	const float r2 = other.radius;

	// this includes other
	if (r1 >= dist + r2)
		return;
	// other includes this
	if (r2 >= dist + r1) {
		*this = other;
		return;
	}

	const glm::vec3 c1 = center, c2 = other.center;

	const glm::vec3 c1toc2 = (c2 - c1) / dist;

	const glm::vec3 p1 = c1 - c1toc2 * r1;
	const glm::vec3 p2 = c2 + c1toc2 * r2;

	center = (p1 + p2) / 2.f;
	radius = glm::distance(p1, p2) / 2.0f;
}

void BoundingSphere::transform(const glm::mat4 &xform) {
	center = glm::vec3(xform * glm::vec4(center, 1.0f));

	glm::vec3 scalesSq = glm::vec3(
		glm::length2(glm::vec3(xform[0])),
		glm::length2(glm::vec3(xform[1])),
		glm::length2(glm::vec3(xform[2])));

	float const maxScaleSq = MAX(scalesSq.x, MAX(scalesSq.y, scalesSq.z));
	float const largestScale = std::sqrt(maxScaleSq);
	radius *= largestScale;
}
