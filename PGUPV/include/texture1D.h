
#ifndef _TEXTURE1DGENERIC_H
#define _TEXTURE1DGENERIC_H 2014

#include "texture.h"
#include "common.h"

namespace PGUPV {

class Image;
    
/**
\class Texture1DGeneric

Esta clase representa un objeto textura 1D. Permite cargar imágenes desde fichero
y establecer los parámetros para su uso.

*/
class Texture1D : public Texture {
public:
	/**
	Constructor por defecto con unos parámetros iniciales (NO corresponden con los
	 valores por defecto de OpenGL

	 */
	Texture1D (GLenum texture_type = GL_TEXTURE_1D, GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
		GLenum wrap_s = GL_REPEAT);
    virtual ~Texture1D () {};
	/**
     Función para cargar en el objeto textura una imagen desde fichero:
		\param filename nombre y ruta (absoluta o relativa) del fichero a cargar
		\return true en caso de haber podido cargar la imagen.
	*/
	virtual bool loadImage(const std::string &filename);
    
    /**
     Función para cargar el objeto Image al objeto textura.
     \param image Imagen a cargar
     \return true si la carga ha tenido éxito
     */
    virtual bool loadImage(const Image &image);
	/**
		Carga al objeto textura desde la zona de memoria indicada una imagen del tamaño indicado. La
        imagen de entrada debe ser obligatoriamente RGBA, de 8 bits por píxel, compacta

		\param pixels Puntero al buffer que contiene la imagen
		\param width Ancho de la imagen, en píxeles
		\param pixels_format Contenido del buffer (GL_RGBA, GL_RED, GL_DEPTH_STENCIL...) Ver Table 8.3 de
			la especificación de OpenGL 4.4
		\param pixels_type Tipo de datos de los elementos del buffer (GL_UNSIGNED_BYTE, GL_FLOAT...) Ver
			Table 8.2 de la especificación de OpenGL 4.4
		\param internalformat Formato interno de la textura (GL_RGBA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL)
	*/
	void loadImageFromMemory(void *pixels, uint width,GLenum pixels_format, 
		GLenum pixels_type, GLint internalformat);
	/**
		Reserva memoria asociada a la textura para almacenar una imagen del tamaño y formato
		especificador.
		\param width Ancho de la imagen, en píxeles
		\param internalformat Formato interno de la image (GL_RGBA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL)
	*/
	void allocate(uint width, GLint internalformat);
	uint getWidth() { return _width;  };
protected:
	void setParams();
	uint _width;
};

};

#endif