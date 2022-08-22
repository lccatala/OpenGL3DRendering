
#ifndef _BINDABLE_TEXTURE_H
#define _BINDABLE_TEXTURE_H 2013

#include <GL/glew.h>
#include <string>

namespace PGUPV  {
	class BindableTexture {
	public:
		explicit BindableTexture(GLenum texture_type);
		virtual ~BindableTexture();
		/**
		 Vincula este objeto textura a la unidad de textura indicada.
		 \param textureUnit Unidad de textura (GL_TEXTURE0, GL_TEXTURE1...)
		 */
		void bind(GLenum textureUnit = GL_TEXTURE0);
		// Desvincula este objeto (la unidad de textura donde estuviera vinculado queda sin textura)
		void unbind();
		// Devuelve el identificador del objeto textura (devuelto por glGenTextures)
		GLuint getId() { return _texId; };
		// Devuelve el tipo de textura (GL_TEXTURE_1D, GL_RECTANGLE_TEXTURE, etc)
		GLenum getTextureType() { return _texture_type; };
		/**
		Borra toda la textura al valor indicado.
		\warning Disponible a partir de GL 4.4. Usa glClearTexImage
		\param level Nivel a borrar
		\param format Formato de los datos apuntados por data (GL_RGBA, GL_R...)
		\param type Tipo de los elementos apuntods por data (GL_FLOAT, GL_INT)...
		\param data puntero al elemento que se usará para borrar la textura. Si es NULL, inicializa a cero
		*/
		virtual void clear(int level, GLenum format, GLenum type, const void *data);
		//! Devuelve el nombre de la textura
		const std::string getName() const { return _name; }
		// Establece el nombre de la textura
		void setName(const std::string &name) { _name = name; }
	protected:
		GLuint _texId;
		bool _ready;
		GLenum _texture_type /*, _texture_type_binding*/;
		int _textureUnitBound;
		std::string _name;
	private:
		// No permitir la copia
		BindableTexture(const BindableTexture &other);
		BindableTexture& operator=(BindableTexture other);
	};
}


#endif
