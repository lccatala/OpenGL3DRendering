#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

namespace PGUPV {
	class Bone {
	public:
		Bone(const std::string &boneName) : name(boneName){};
		const std::string &getName() const { return name; }
		void setMatrix(const glm::mat4 &m);
		const glm::mat4 &getMatrix() const;
		void addWeight(uint32_t vertexIndex, float weight);
		size_t getNWeights() const {
			return weights.size();
		}
		struct VertexWeight {
			uint32_t vertexIndex;
			float weight;
		};
		const VertexWeight &getWeight(uint32_t index) const {
			return weights[index];
		}

	private:
		std::string name;
		glm::mat4 matrix;
		std::vector<VertexWeight> weights;
	};
};