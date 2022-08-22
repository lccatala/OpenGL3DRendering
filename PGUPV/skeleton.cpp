#include <gsl/gsl>

#include "log.h"

#include "skeleton.h"
#include "bone.h"

using PGUPV::Skeleton;
using PGUPV::Bone;

void Skeleton::addBone(std::shared_ptr<Bone> bone) {
	boneNameCache[bone->getName()] = gsl::narrow<uint32_t>(bones.size());
	bones.push_back(bone);
}

void addWeight(std::vector<glm::uvec4> &boneIds, std::vector<glm::vec4> &boneWeights, uint32_t bone, uint32_t vertex, float weight) {
	assert(vertex < boneIds.size());

	for (unsigned int i = 0; i < 4; i++) {
		if (boneWeights[vertex][i] == 0.0f) {
			boneWeights[vertex][i] = weight;
			boneIds[vertex][i] = bone;
			return;
		}
	}
	WARN("The maximum number of bones per vertex is 4. Ignoring bones");
}

uint32_t Skeleton::getNBones() const
{
	return gsl::narrow<uint32_t>(bones.size());
}

void Skeleton::buildArraysOfVerticesAndWeights(uint32_t size, std::vector<glm::uvec4>& boneIds, std::vector<glm::vec4>& boneWeights)
{
	boneIds.clear();
	boneIds.resize(size, glm::uvec4(0U));
	boneWeights.clear();
	boneWeights.resize(size, glm::vec4(0.0f));

	for (size_t i = 0; i < bones.size(); i++) {
		const auto &bone = *bones[i];
		for (size_t j = 0; j < bone.getNWeights(); j++) {
			auto vw = bone.getWeight(gsl::narrow<uint32_t>(j));
			addWeight(boneIds, boneWeights, gsl::narrow<uint32_t>(i), vw.vertexIndex, vw.weight);
		}
	}
}

//void Skeleton::buildArrayOfMatrices(std::vector<glm::mat4>& matrices)
//{
//	matrices.clear();
//	matrices.resize(bones.size());
//
//	for (size_t i = 0; i < bones.size(); i++) {
//		matrices[i] = bones[i]->getMatrix();
//	}
//}
