#include <GL/glew.h>
#include "log.h"
#include "utils.h"


static GLenum lastPolygonMode = GL_FILL;

void glPolygonMode2(GLenum face, GLenum mode) {
	if (face != GL_FRONT_AND_BACK)
		ERRT("glPolygonMode sólo soporta GL_FRONT_AND_BACK en el primer parámetro");
	lastPolygonMode = mode;
	glPolygonMode(face, lastPolygonMode);
}

void glGetIntegerv2(GLenum what, GLint *ptr) {
	if (what == GL_POLYGON_MODE && PGUPV::getManufacturer() != PGUPV::Manufacturer::NVIDIA
		&& PGUPV::getManufacturer() != PGUPV::Manufacturer::INTEL) 
		*ptr = lastPolygonMode;
	else
		glGetIntegerv(what, ptr);
}
