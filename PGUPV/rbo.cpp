#include <sstream>

#include "rbo.h"
#include "utils.h"
#include "log.h"

using PGUPV::RBO;
using PGUPV::RBOFactory;
using std::shared_ptr;
using std::string;

RBOFactory::RBOFactory() {
	_width = 0;
	_height = 0;
	_internalFormat = GL_RGBA;
	_samples = 0;
}

RBOFactory &RBOFactory::width(uint width) {
	_width = width;
	return *this;
}

RBOFactory &RBOFactory::height(uint height) {
	_height = height;
	return *this;
}

RBOFactory &RBOFactory::internalFormat(GLenum internalFormat) {
	_internalFormat = internalFormat;
	return *this;
}

RBOFactory &RBOFactory::samples(uint samples) {
	_samples = samples;
	return *this;
}

shared_ptr<RBO> RBOFactory::build() {
//	return std::make_shared<RBO>(_width, _height, _internalFormat, _samples);
	return std::shared_ptr<RBO>(new RBO(_width, _height, _internalFormat, _samples));
}


RBO::RBO(uint width, uint height, GLenum internalFormat, uint samples) {
	CHECK_GL();
	glGenRenderbuffers(1, &_rboId);
	bind();
	if (samples > 0) 
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

	std::ostringstream msg;
	msg << "RBO(" << width << ", " << height << ", " << internalFormat << ", " << samples << ")";
	CHECK_GL2("Error al construir el RenderBuffer con " + msg.str());
	
	INFO("Renderbuffer creado con id " + std::to_string(_rboId) + " (" + msg.str() + ")");

}

RBO::~RBO() {
	glDeleteRenderbuffers(1, &_rboId);
	INFO("Renderbuffer liberado con id " + std::to_string(_rboId));
}

void RBO::bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, _rboId);
}
