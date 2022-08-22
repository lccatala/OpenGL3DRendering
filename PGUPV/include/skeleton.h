#pragma once

#include <string>
#include <map>
#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <limits>

namespace PGUPV {
	class Bone;

	class Skeleton {
	public:
		void addBone(std::shared_ptr<Bone> bone);

		static constexpr uint32_t NOBONE = std::numeric_limits<uint32_t>::max();

		uint32_t getBoneIndex(const std::string &name) {
			auto it = boneNameCache.find(name);
			if (it != boneNameCache.end()) {
				return it->second;
			}
			return NOBONE;
		}

		std::shared_ptr<Bone> getBone(uint32_t i) {
			return bones[i];
		}

		std::shared_ptr<Bone> getBone(const std::string &name) {
			uint32_t bindx = getBoneIndex(name);
			if (bindx != std::numeric_limits<uint32_t>::max()) {
				return bones[bindx];
			}
			return std::shared_ptr<Bone>();
		}

		uint32_t getNBones() const;

		void buildArraysOfVerticesAndWeights(uint32_t size, std::vector<glm::uvec4> &boneIds, std::vector<glm::vec4> &boneWeights);
		//void buildArrayOfMatrices(std::vector<glm::mat4> &matrices);
	private:
		std::map<std::string, uint32_t> boneNameCache;
		std::vector<std::shared_ptr<Bone>> bones;
	};
};