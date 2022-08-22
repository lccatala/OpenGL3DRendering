
#ifndef _TEXTURECUBEMAP_H
#define _TEXTURECUBEMAP_H 2011

#include <iostream>
#include <GL/glew.h>
#include "common.h"
#include "texture.h"

namespace PGUPV {

/*
\class TextureCubeMap

Esta clase representa un mapa cúbico. Permite cargar imágenes desde fichero
y establecer los parámetros para su uso.

*/
class TextureCubeMap : public Texture {
public:
  /**
  Constructor por defecto con unos parámetros iniciales (NO corresponden con
   los valores por defecto de OpenGL
   */
  TextureCubeMap(GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
                 GLenum wrap_s = GL_CLAMP_TO_EDGE,
                 GLenum wrap_t = GL_CLAMP_TO_EDGE);
  /**
  Función para cargar una imagen desde fichero:
  \param face: la cara del cubo que se está cargando
     (GL_TEXTURE_CUBE_MAP_POSITIVE_X...)
  \param flipV: reflejar la imagen verticalmente
  \param filename: nombre y ruta (absoluta o relativa) del fichero a cargar
  \param error_output: un flujo de salida donde escribir los posibles errores
      que se produzcan. Puede ser un fichero, el flujo estándar de error
     (es el valor por defecto), o NULL, para no generar ningún error.
  \return true en caso de haber podido cargar la imagen.
  */
  bool loadImage(GLenum face, std::string filename, bool flipV = true,
                 std::ostream *error_output = &std::cerr);
  /**
  Función para cargar todas las imágenes del mapa cúbico de una vez. Hay que
  proporcionar el nombre base de los ficheros, y debe seguir el siguiente
  formato:

  filename = base.png

  Se intentará cargar los ficheros:

   baseposx.png
   basenegx.png
   baseposy.png
          ...
   \param filename Nombre base de los ficheros, incluyendo la extensión
   \param flipV invertir la imagen verticalmente
   \param error_output flujo donde escribir los posibles errores que se
      produzcan
   \return true si ha podido cargar todas las imágenes del mapa cúbico
  */
  bool loadImages(std::string filename, bool flipV = true,
                  std::ostream *error_output = &std::cerr);
  /* Carga un mapa cúbico almacenado en un fichero dds */
  bool loadDDS(std::string filename, std::ostream *error_output = &std::cerr);

private:
  // Bits menos significativos:
  // ... NEG_Z POS_Z NEG_Y POS_Y NEG_X POS_X
  uint loadedFaces;
};
};

#endif
