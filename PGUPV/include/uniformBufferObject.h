#ifndef _UNIFORM_BUFFER_OBJECT_H
#define _UNIFORM_BUFFER_OBJECT_H 2013

#include "bufferObject.h"

#include "common.h"

namespace PGUPV {

/* 
	Esta clase es la base para construir un uniform buffer object. Esta clase es virtual porque
	tiene dos miembros virtuales puros, que hay que implementar en la clase concreta
	que derive de UniformBufferObject.
	El miembro getBlockName devuelve el nombre de la bloque uniform, getDefinition devuelve la 
	definición del bloque en GLSL.
	Antes de compilar un shader, se sustituirán las líneas que contengan un $ seguido del nombre 
	devuelto por getBlockName por el texto devuelto por getDefinition.
	Ejemplos de clases derivadas: UBOMaterial, UBOLightSources o GLMatrices
*/
class UniformBufferObject : public BufferObject {
public:
	virtual const std::string &getBlockName() const = 0;
	virtual const Strings &getDefinition() const = 0;
protected:
	explicit UniformBufferObject(ulong size) : BufferObject(size, GL_DYNAMIC_DRAW) {};
	virtual ~UniformBufferObject() {};
	// Prohibido copiar
	UniformBufferObject(const UniformBufferObject &) = delete;
};

};


#endif