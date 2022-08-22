#include "bone.h"

using PGUPV::Bone;

void Bone::setMatrix(const glm::mat4 &m) {
	matrix = m;
}

const glm::mat4 &Bone::getMatrix() const
{
	return matrix;
}

void Bone::addWeight(uint32_t vertexIndex, float weight) {
	weights.push_back({ vertexIndex, weight });
}
