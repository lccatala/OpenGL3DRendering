#ifndef _HACKS_H
#define _HACKS_H 2014

#include <GL/glew.h>

/* 
En este fichero se intenta arreglar alguna chapucilla de OpenGL.

Por ejemplo, el siguiente código debería funcionar, según la especficación de OpenGL 4.5 (ver tabla 23.10), pero AMD
y MAC OS no reconocen la constante GL_POLYGON_MODE 
		
		GLint pm[2];
		CHECK_GL();
		glGetIntegerv(GL_POLYGON_MODE, pm);
		CHECK_GL();
*/

#ifdef glPolygonMode 
#undef glPolygonMode
#endif

#ifdef glGetIntegerv
#undef glGetIntegerv
#endif


#define glPolygonMode glPolygonMode2
#define glGetIntegerv glGetIntegerv2

void glPolygonMode2(GLenum face, GLenum mode);
void glGetIntegerv2(GLenum what, GLint *ptr);

#endif
