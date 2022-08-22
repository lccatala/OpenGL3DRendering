
#ifndef _TRANSFORMFEEDBACKOBJECT_H
#define _TRANSFORMFEEDBACKOBJECT_H 2013

/** \file transformFeedbackObject.h

\author Paco Abad

*/

#include "common.h"

namespace PGUPV {

/**
\class TransformFeedbackObject 

Clase que representa un transform feedback object (TAO) en una implementación OpenGL.

Un TAO mantiene el estado del proceso de realimentación de transformaciones. 
Mantienen el estado de la captura de información de vértices en buffer objects (qué buffers
están vinculados a los puntos de vinculación del proceso de transform feedback).
*/

class TransformFeedbackObject {
public:
	TransformFeedbackObject();
	~TransformFeedbackObject();
	void bind();
	void unbind();
private:
	// Prohibir la copia
	TransformFeedbackObject(const TransformFeedbackObject &);
	TransformFeedbackObject& operator=(TransformFeedbackObject);
	uint tao;
};
};

#endif