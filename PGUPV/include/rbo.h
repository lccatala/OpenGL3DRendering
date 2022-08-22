#ifndef _RENDERBUFFER_H
#define _RENDERBUFFER_H 2013

#include <memory>
#include <GL/glew.h>
#include "common.h"

namespace PGUPV {
class RBOFactory;

class RBO {
public:
	~RBO();
	// Vincula el renderbuffer al punto de vinculación GL_RENDERBUFFER
	void bind();
	// Devuelve el identificador del Renderbuffer
	GLuint getId() {return _rboId; };
private:
	RBO(uint width, uint height, GLenum internalFormat, uint samples);
	GLuint _rboId;
	friend class RBOFactory;
};

class RBOFactory {
public:
	RBOFactory();
	RBOFactory &width(uint width);
	RBOFactory &height(uint height);
	RBOFactory &internalFormat(GLenum internalFormat);
	RBOFactory &samples(uint samples);
	std::shared_ptr<RBO> build();
private:
	uint _width, _height;
	GLenum _internalFormat;
	uint _samples;
};

};

#endif
