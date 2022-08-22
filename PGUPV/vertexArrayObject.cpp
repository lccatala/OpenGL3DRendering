#include "vertexArrayObject.h"

#include <GL/glew.h>

using PGUPV::VertexArrayObject;

VertexArrayObject::VertexArrayObject() {
	glGenVertexArrays(1, &vao);
}

VertexArrayObject::~VertexArrayObject() {
	glDeleteVertexArrays(1, &vao);
}

void VertexArrayObject::bind() {
	glBindVertexArray(vao);
}

void VertexArrayObject::unbind() {
	glBindVertexArray(0);
}
