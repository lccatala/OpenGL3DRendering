
#ifndef _VERTEXARRAYOBJECT_H
#define _VERTEXARRAYOBJECT_H 2013

/** \file bufferObject.h

\author Paco Abad

*/

#include "common.h"

namespace PGUPV {

/**
\class VertexArrayObject 

Clase que representa un vertex array object (VAO) en una implementación OpenGL.

Un VAO almacena la configuración de los atributos de un modelo (qué atributo está conectado
a qué buffer object, qué atributos tienen valores estáticos y qué valores, etc).
Normalmente habrá un VAO por malla.

*/

class VertexArrayObject {
public:
	VertexArrayObject();
	~VertexArrayObject();
	void bind();
	void unbind();
private:
	// Prohibir la copia
	VertexArrayObject(const VertexArrayObject &);
	VertexArrayObject& operator=(VertexArrayObject);
	uint vao;
};
};

#endif