#ifndef _IMAGE_H
#define _IMAGE_H

#include <string>
#include <GL/glew.h>
#include <FreeImage.h>

#include "common.h"



namespace PGUPV {
	/**

	\class Image
	Esta clase representa una imagen, que normalmente se carga desde un fichero.
	\warning No confundir con las Images de OpenGL que se usan para escribir en
	ellas desde un shader (se usa glBindImageTexture desde CPU para definirlas)

	*/

	class Image {
	public:
    Image(Image &&other);
		// Carga la imagen desde el fichero indicado
		explicit Image(std::string filename);
		// Crea una imagen con el tamaño indicado. Opcionalmente, copia la información
		// apuntada por data
		Image(uint width, uint height, uint bpp, void *data = NULL);
		// Carga la imagen en el fichero indicado. La imagen que contenía este objeto
		// se destruye (incluyendo el puntero que devolvería Image::getPixels)
		~Image();
		bool load(std::string filename);
		/**
	  Guarda la imagen en el fichero indicado. Se puede guardar un frame de una animación
	  o una cara de un cubo
	  \param filename Nombre del fichero resultante
	  \param frame cara o frame a guardar
	  */
		bool save(const std::string &filename, uint frame = 0);
		
		/**
		Guarda en el fichero indicado la imagen definida por el resto de parámetros
		*/
		static bool save(const std::string &filename, uint width, uint height, uint bpp, uint8_t *bytes);
		static bool saveHDR(const std::string& filename, uint32_t width, uint32_t height, uint32_t bpp, const float* bytes);

		// Ancho
		uint getWidth() const { return _width; };
		// Alto
		uint getHeight() const { return _height; };
		// Bits por píxel
		uint getBPP() const { return _bpp; };
		// Invierte la imagen verticalmente
		void flipV();
		// Número de caras (6 si la imagen es un mapa de entorno cúbico, 1 si es una
		// imagen normal)
		uint getNumFaces() const { return _nfaces; };
		/**
		\return Número de frames de la animación
		*/
		uint getAnimationFrames() const { return _nAnimationFrames; };
		/**
		 Devuelve un nuevo objeto Image, con el frame indicado.
		 \param frame Número de frame a extraer (de 0 a getAnimationFrames() - 1)
		 */
		Image *extractFrame(uint frame) const;
		// Devuelve el tamaño en bytes de una fila de píxeles de la image
		uint getStride() const { return _stride; };
		/**
		Compara la similitud de otra imagen con la actual. Se considera que dos
		imágenes son iguales si la máxima diferencia entre cualquier canal de color de
		cualquier píxel visible es menor o igual a la especificada. Por defecto deben
		ser exactamente iguales.
		\param other La otra imagen
		\param maxDifference Máxima diferencia en cualquier canal de color (0-255)
		\return true, si la diferencia entre cualquier par de píxeles es menor
		o igual que la indicada
		*/
		bool equals(Image &other, uint maxDifference = 0);

		/**
		Devuelve una nueva imagen con la diferencia entre this y la imagen
		proporcionada. Las imágenes deben ser del mismo tamaño.
		Se devuelve una imagen RGBA, donde cada pixel contiene la diferencia entre los
		dos píxeles originales por canal de color.
		\param other La otra imagen
		\param ignoreAlpha si true, ignora las diferencias en el canal alfa, y la imagen resultante
		  no tendrá canal alfa
		\return Una nueva imagen con la diferencia entre ambas.
		*/
		Image *difference(Image &other, bool ignoreAlpha = true);

		/**
		Devuelve un puntero al píxel indicado. El origen de la imagen está en la
		esquina inferior izquierda.

		\param x coordenada x del píxel
		\param y coordenada y del píxel
		\param layer capa a devolver (en caso de que la imagen sea un mapa cúbico, o
		una animación)
		\return un puntero a los datos del píxel
		*/
		void *getPixels(uint x = 0, uint y = 0, uint layer = 0) const;

		/**
		\return la constante de GL que describe el contenido de la imagen (GL_RED, GL_RG,
		  GL_RGB o GL_RGBA)
		*/
		GLenum getGLFormatType() const;

		/**
		\return la constante de GL que describe el tipo de datos básico de los componentes de la
		imagen (GL_UNSIGNED_BYTE, GL_FLOAT, etc)
		*/
		GLenum getGLPixelBaseType() const;
		/**
		\return la constante de GL que mejor describe el contenido de la imagen (puede ser de componentes
		tipo float para HDR)
		*/
		GLenum getSuggestedGLInternalFormatType() const;

    /**
    Convierte una imagen de entrada que tiene 8 bpp en una imagen en escala de grises con 24 bits por pixel.
    Para cada pixel replica el valor original para RGB.
    */
    static Image convert8BPPGrayTo24BPPGray(const Image &src);

		/**
		\return Información sobre la versión de la biblioteca de carga de imágenes utilizada
		*/
		static const std::string getLibraryInfo();
	private:
		Image(const Image &);
		Image &operator=(const Image &);
		bool loadDDS(const std::string &filename);

		bool loadSimple(const std::string &filename, const ::FREE_IMAGE_FORMAT fileType);
		bool loadMulti(const std::string &filename, const ::FREE_IMAGE_FORMAT fileType);
		void loadFrameFromMulti(const unsigned int frame) const;

		void swapRB(uint frame) const;
		bool loadFreeImage(::FIBITMAP *image, const uint frame = 0);
		static bool _freeImageInitialized;
		void releaseMemory();
		static void initLib();
		uint _width, _height;
		mutable uchar **_data;
		mutable std::vector<::FIBITMAP *> lockedPages;
		uint _bpp;
		uint _nfaces;
		uint _nAnimationFrames;
		uint _stride;
		std::string _filename;

		::FIBITMAP* freeimageImage;
		::FIMULTIBITMAP *freeimageMultiImage;



	};
};

#endif
