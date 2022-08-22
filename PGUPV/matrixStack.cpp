
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "matrixStack.h"
#include "common.h"
#include "log.h"
#include "utils.h"

using PGUPV::MatrixStack;

MatrixStack::MatrixStack() {
	 matrix.push_back(glm::mat4(1.0f));
}

void MatrixStack::reset() {
	matrix.clear();
	matrix.push_back(glm::mat4(1.0f));
}

void MatrixStack::loadIdentity() {
	matrix[matrix.size() - 1] = glm::mat4(1.0f);
}

void MatrixStack::pushMatrix() {
	if (matrix.size() > MAX_MATRIXSTACK_DEPTH) 
		ERRT("Se han apilado más de " + std::to_string(MAX_MATRIXSTACK_DEPTH) + " matrices");
	matrix.push_back(matrix.back());
}

void MatrixStack::popMatrix() {
	if (matrix.size() > 1) 
		matrix.pop_back();
	else
		ERRT("No se puede desapilar de una pila vacía");
}

void MatrixStack::translate(float x, float y, float z) {
	translate(glm::vec3(x, y, z));
}

void MatrixStack::translate(const glm::vec3 &t) {
	glm::mat4 &mat = matrix[matrix.size()-1];
	
	mat = glm::translate(mat, t);
}


void MatrixStack::rotate(float radians, float axis_x, float axis_y, float axis_z) {
  rotate(radians, glm::vec3(axis_x, axis_y, axis_z));
}

void MatrixStack::rotate(float radians, const glm::vec3 &axis) {
	glm::mat4 &mat = matrix[matrix.size()-1];
	
  mat = glm::rotate(mat, radians, axis);
}


void MatrixStack::scale(float sx, float sy, float sz) {
	scale(glm::vec3(sx, sy, sz));
}

void MatrixStack::scale(const glm::vec3 &s) {
	glm::mat4 &mat = matrix[matrix.size()-1];

	mat = glm::scale(mat, s);
}

void MatrixStack::multMatrix(const glm::mat4 &m) {
	glm::mat4 &mat = matrix[matrix.size() - 1];

	mat = mat * m;
}


void MatrixStack::setMatrix(const glm::mat4 &m) {
	
	matrix[matrix.size()-1] = m;
}

const glm::mat4 &MatrixStack::getMatrix() const {
	return matrix.back();
}

std::ostream& PGUPV::operator<<(std::ostream &os, const MatrixStack& s) {
	os << "The stack has " << s.size() << " matrices\n";
	os << "Top Matrix:\n";
	os << PGUPV::to_string(s.getMatrix());
	return os;
}