#ifndef _MATRIX_STACK
#define _MATRIX_STACK 2011

#include <glm/glm.hpp>
#include <vector>
#include <ostream>

#include "common.h"

namespace PGUPV {

// Este valor es básicamente para detectar desequilibrios en el número de push y 
// 	pops, no porque no podamos apilar más de este número de elementos...
#define MAX_MATRIXSTACK_DEPTH 128
	class MatrixStack {
	public:
		MatrixStack();
		void loadIdentity();
		void pushMatrix();
		void popMatrix();
		void translate(float x, float y, float z);
		void translate(const glm::vec3 &t);
		void rotate(float radians, float axis_x, float axis_y, float axis_z);
    void rotate(float radians, const glm::vec3 &axis);
		void scale(float sx, float sy, float sz);
		void scale(const glm::vec3 &s);
		void multMatrix(const glm::mat4 &m);
		void setMatrix(const glm::mat4 &m);
		const glm::mat4 &getMatrix() const;
		void reset();
		size_t size() const { return matrix.size(); };
	private:
		std::vector<glm::mat4> matrix;
		friend std::ostream& operator<<(std::ostream &os, const MatrixStack& s);
	};
	std::ostream& operator<<(std::ostream &os, const MatrixStack& s);
};

#endif

