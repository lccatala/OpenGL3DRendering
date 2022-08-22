
#ifndef _TEXTURE3DGENERIC_H
#define _TEXTURE3DGENERIC_H 2014

#include <iostream>
#include <GL/glew.h>
#include "texture.h"
#include "common.h"

namespace PGUPV {

class Image;
    
/* Esta clase representa un objeto textura 3D. Permite cargar imágenes desde fichero
y establecer los parámetros para su uso.
*/
class Texture3DGeneric : public Texture {
public:
	/**
		Constructor por defecto con unos parámetros iniciales (NO corresponden con los
	 valores por defecto de OpenGL
		\param  texture_type Constante de OpenGL que identifica el tipo de textura (GL_TEXTURE_3D, 
		GL_TEXTURE_2D_ARRAY...)
		\param minfilter Filtro de minimización (por defecto, GL_LINEAR)
		\param magfilter Filtro de maximización (por defecto, GL_LINEAR)
		\param wrap_s Modo de repetición de la textura en la dirección s (por defecto GL_REPEAT)
		\param wrap_t Modo de repetición de la textura en la dirección t (por defecto GL_REPEAT)
		\param wrap_r Modo de repetación de la textura en la dirección r (por defecto GL_REPEAT)
	 */
	Texture3DGeneric(GLenum texture_type, GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
		GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT, GLenum wrap_r = GL_REPEAT);
    virtual ~Texture3DGeneric() {};
	/**
     Función para cargar en el textura una imagen (o imágenes) desde fichero. Si el fichero contiene
     varios frames de animación (por ejemplo, un GIF animado), cargará cada frame en una capa.
		\param filename nombre y ruta (absoluta o relativa) del fichero a cargar
		\return true en caso de haber podido cargar la imagen.
	*/
	virtual bool loadImage(const std::string &filename);

    /**
     Función para cargar en el textura una imagen desde el objeto Image. Si dicho objeto contiene
     varios frames de animación, cargará cada frame en una capa.
     \param image objeto Image con la imagen a cargar en la textura
     */
    virtual void loadImage(const Image &image);
    /**
     Función para cargar en una capa de la textura la imagen dada.
     \param image imagen con la capa a cargar
     \param slice el número de capa a cargar
     */
    virtual void loadSlice(const Image &image, uint slice);
	/**
		Carga desde la zona de memoria indicada una imagen del tamaño indicado. La imagen de entrada debe
		ser obligatoriamente RGBA, de 8 bits por píxel, compacta.
        Se cargará en la capa indicada.

		\param pixels Puntero al buffer que contiene la imagen
		\param width Ancho de la imagen, en píxeles
		\param height Alto de la imagen, en píxeles
		\param slice número de capa donde cargar la imagen
		\param pixels_format Contenido del buffer (GL_RGBA, GL_RED, GL_DEPTH_STENCIL...) Ver Table 8.3 de
			la especificación de OpenGL 4.4
		\param pixels_type Tipo de datos de los elementos del buffer (GL_UNSIGNED_BYTE, GL_FLOAT...) Ver
			Table 8.2 de la especificación de OpenGL 4.4
		\param internalformat Formato interno de la textura (GL_RGBA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL)
	*/
	void loadSlice(void *pixels, uint width, uint height, uint slice, GLenum pixels_format,
		GLenum pixels_type, GLint internalformat);
	/**
		Reserva memoria asociada a la textura para almacenar una imagen del tamaño y formato
		especificador.
		\param width Ancho de la imagen, en píxeles
		\param height Alto de la imagen, en píxeles
		\param depth Número de capas o frames de animación
		\param internalformat Formato interno de la image (GL_RGBA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL)
	*/
	void allocate(uint width, uint height, uint depth, GLint internalformat);
	uint getWidth() const { return _width;  };
	uint getHeight() const { return _height;  };
	uint getDepth() const { return _depth; };
protected:
	void setParams();
	uint _width, _height, _depth;
};

};

#endif