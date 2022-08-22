#ifndef _BINDING_POINT_H
#define _BINDING_POINT_H 2014

#include <memory>
#include <GL/glew.h>

#include "common.h"   // for ulong, uint

namespace PGUPV {
  class BufferObject;

	class BindingPoint {
	public:
		explicit BindingPoint(GLenum GL_BP) : GL_bindingPoint(GL_BP){};
		/** Vincula el B.O. al punto de vinculación indicado.
		\returns un puntero compartido (que puede estar vacío) al B.O. que estaba
		previamente vinculado a este punto
		*/
		std::shared_ptr<BufferObject> bind(std::shared_ptr<BufferObject> bo);
		/**
		Vincula el B.O. 0 (es decir, desvincula el BO previamente vinculado)
		\returns un puntero compartido (que puede estar vacío) al B.O. que estaba
		previamente vinculado a este punto
		*/
		std::shared_ptr<BufferObject> unbind();
		/** Escribe en la zona de memoria apuntada por dst el contenido del B.O.
		Se copiará el contenido completo del buffer (asegúrate de tener bastante
		espacio). Lanza una excepción si no hay un buffer vinculado.
		*/
		void read(void *dst);
		/**
		Escribe en la zona de memoria apuntada por dst una parte del contenido del
		B.O. vinculado.
		\param dst: buffer donde escribir
		\param size: número de bytes a leer (si 0, se leerá hasta el final del buffer)
		\param offset: primer byte a leer
		*/
		void read(void *dst, size_t size, uint offset);
		/** Escribe al B.O. vinculado los datos apuntados por src. Escribirá el número
		   de
		   bytes que se indicó al crear el buffer. Si no hay B.O. asociado, se
		   producirá una excepción */
		void write(const void *src);
		/** Escribe en una parte del B.O. vinculado los datos apuntados por src. Se
		 empieza
		 a escribir desde la posición 'offset' bytes desde el inicio del B.O.
		 y se escriben 'size' bytes. Si no hay B.O. asociado, se producirá una
		 excepción  */
		void write(const void *src, ulong size, ulong offset);
		/** Copia una porción del buffer vinculado al punto de vinculación
		GL_COPY_READ_BUFFER al buffer vinculado a GL_COPY_WRITE_BUFFER
		\param readOffset posición del primer byte a leer en el buffer de lectura
		\param writeOffset posición del primer byte a escribir en el buffer de
		escritura
		\param size número de bytes a copiar
		*/
		static void copy(GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
		/**
		Devuelve un puntero en memoria de la CPU que se corresponde al contenido del B.O.
		vinculado al punto de vinculación.
		\param usage indica el uso que se le va a dar al puntero (GL_READ_ONLY,
		GL_WRITE_ONLY, o GL_READ_WRITE)
		\return un puntero en memoria de CPU
		*/
		void *map(GLenum usage);
		/**
		Devuelve un puntero en memoria de la CPU que se corresponde a una parte del contenido del B.O.
		vinculado al punto de vinculación.
		\param offset Inicio de la zona a mapear
		\param size Tamaño de la zona a mapear
		\param access una combinación de las siguientes constantes: GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, 
		GL_MAP_PERSISTENT_BIT, GL_MAP_COHERENT_BIT, GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT, 
		GL_MAP_FLUSH_EXPLICIT_BIT, GL_MAP_UNSYNCHRONIZED_BIT
		\return un puntero en memoria de CPU
		*/
		void *map(ulong offset, ulong size, GLbitfield access);
		/**
		Termina el mapeo en memoria de CPU del B.O. vinculado al punto de vinculación. Si
		se mapeó el buffer para escritura, esta llamada le indicará a OpenGL que los datos 
		están listos. Cada llamada a map debería tener su correspondiente unmap.
		*/
		void unmap();

		/**
		Inicializa (una parte) del buffer vinculado al punto de vinculación al valor indicado
		por data. Ver: http://docs.gl/gl4/glClearBufferSubData

		\param internalformat el formato interno con el que se guardará la información en el B.O.
		 (p.e. GL_R32F, GL_RGBA32F, etc) Debe ser un formato con tamaño 
		\param offset inicio de la zona del B.O. a inicializar (en bytes)
		\param size tamaño en bytes de la zona a inicializar
		\param format el formato de la memoria apuntada por data (p.e., GL_RED, GL_RG, GL_RGBA, GL_RED_INTEGER
		\param type el tipo de datos de la memoria apuntado por data (GL_FLOAT, GL_UNSIGNED_INT, GL_BYTE...)
		\param data Un puntero al valor con el que inicializar el buffer
		\warning offset y size deben ser múltiplos enteros del tamaño de internalformat
		*/
		void clear(GLenum internalformat, GLintptr offset, GLsizeiptr size,
			GLenum format, GLenum type, const void *data);

	protected:
		GLenum GL_bindingPoint;
		std::weak_ptr<BufferObject> bound;
	};

	extern BindingPoint gl_array_buffer,
		gl_copy_read_buffer,
		gl_copy_write_buffer,
		gl_element_array_buffer,
		gl_pixel_pack_buffer,
		gl_pixel_unpack_buffer,
		gl_query_buffer, 
    gl_texture_buffer,
		gl_draw_indirect_buffer, // GL 4.0
		gl_dispatch_indirect_buffer; // GL 4.3
};
#endif
