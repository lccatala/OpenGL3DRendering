
#ifndef _MESH_H
#define _MESH_H 2011

#include <memory>
#include <vector>
#include <GL/glew.h>

#include "common.h"
#include "boundingVolumes.h"
#include "vertexArrayObject.h"
#include "baseMaterial.h"

namespace PGUPV {

	/**

	\class Mesh

	Clase que contiene una malla de un modelo. Un modelo (definido por la clase
	Model) puede estar compuesto por varias
	mallas. Una malla está compuesta normalmente por varios arrays (posiciones de
	vértices, normales, colores,
	coordenadas de textura, etc). Como se están definiendo los atributos de una
	serie de vértices, todos los arrays
	deberían tener el mismo tamaño, excepto el array de índices, que puede tener un
	número de elementos distinto.
	Cada array se almacenará en un VBO y las conexiones con los puntos de
	vinculación de atributos se almacena en un VAO por
	cada Mesh.
	Para dibujar primitivas con los vértices y sus atributos anteriores, se pueden
	incorporar llamadas de dibujado con
	el método addDrawCommand. De esta forma, con un mismo array de vértices se
	pueden ejecutar varias órdenes de dibujado.
	Por ejemplo, para dibujar un triángulo, se podría crear la siguiente Malla:

	GLfloat vertices[] = {-0.5, -0.5, 0.0,
	0.5, -0.5, 0.0,
	0.0, 0.5, 0.0};
	GLfloat colores[] = {1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0};

	auto m = std::make_shared<Mesh>();;

	m->addColors(colores, 3, 3);
	m->addVertices(vertices, 3, 3);
	m->addDrawCommand(new PGUPV::DrawArrays(GL_TRIANGLES, 0, 3));
	*/

	class UniformBufferObject;
	class BindableTexture;
	class DrawCommand;
	class BufferObject;
	class UBOBones;
	class Skeleton;

	class Mesh {
	public:
		// Usar siempre estos puntos de vinculación para los atributos
		enum BufferObjectType {
			VERTICES,
			NORMALS,
			COLORS,
			INDICES,
			TEX_COORD0,
			TEX_COORD1,
			TEX_COORD2,
			TEX_COORD3,
			TANGENTS,
			BONE_IDS,
			BONE_WEIGHTS,
			_LAST_
		};

#define NUM_TEX_COORD 4
		// Constructor de una malla
		Mesh();
		~Mesh();
		// Devuelven los volúmenes de inclusión de la malla
		BoundingBox getBB() const { return bb; };
		BoundingSphere getBS() const { return bs; };
		// Devuelve la posición del centro de la caja de inclusión
		glm::vec3 center() const;

		// Define la posición de los vértices que componen la malla. Recibe un puntero
		// a un vector de n elementos de tipo vec2
		void addVertices(const glm::vec2 *v, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define la posición de los vértices que componen la malla. Recibe un puntero
		// a un vector de n elementos de tipo vec3
		void addVertices(const glm::vec3 *v, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define la posición de los vértices que componen la malla. Recibe un puntero
		// a un vector de n elementos de tipo vec4
		void addVertices(const glm::vec4 *v, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define la posición de los vértices que componen la malla. Recibe un puntero
		// a un vector de n elementos, donde
		// cada elemento está compuesto por 'ncomponents' floats (normalmente 3 o 4)
		void addVertices(const float *v, uint ncomponents, size_t nVertices,
			GLenum usage = GL_STATIC_DRAW);
		void addVertices(const std::vector<glm::vec2> &v,
			GLenum usage = GL_STATIC_DRAW) {
			addVertices(&v[0], v.size(), usage);
		};
		void addVertices(const std::vector<glm::vec3> &v,
			GLenum usage = GL_STATIC_DRAW) {
			addVertices(&v[0], v.size(), usage);
		};
		void addVertices(const std::vector<glm::vec4> &v,
			GLenum usage = GL_STATIC_DRAW) {
			addVertices(&v[0], v.size(), usage);
		};
		// Define los índices para dibujar la malla. Cada índice es de tipo unsigned
		// byte. Recibe un vector de n índices.
		void addIndices(const GLubyte *i, size_t n, GLenum usage = GL_STATIC_DRAW);
		void addIndices(const std::vector<GLubyte> &i,
			GLenum usage = GL_STATIC_DRAW) {
			addIndices(&i[0], i.size(), usage);
		};
		// Define los índices para dibujar la malla. Cada índice es de tipo unsigned
		// short. Recibe un vector de n índices.
		void addIndices(const GLushort *i, size_t n, GLenum usage = GL_STATIC_DRAW);
		void addIndices(const std::vector<GLushort> &i,
			GLenum usage = GL_STATIC_DRAW) {
			addIndices(&i[0], i.size(), usage);
		};
		// Define los índices para dibujar la malla. Cada índice es de tipo unsigned
		// int. Recibe un vector de n índices.
		void addIndices(const GLuint *i, size_t n, GLenum usage = GL_STATIC_DRAW);
		void addIndices(const std::vector<GLuint> &i, GLenum usage = GL_STATIC_DRAW) {
			addIndices(&i[0], i.size(), usage);
		};
		// Define las normales para dibujar la malla. Cada normal es de tipo vec3.
		// Recibe un vector de n normales.
		void addNormals(const glm::vec3 *nr, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define las normales para dibujar la malla. Cada normal está definida por 3
		// floats consecutivos. Recibe un vector de n
		// normales.
		void addNormals(const GLfloat *nr, size_t n, GLenum usage = GL_STATIC_DRAW);
		void addNormals(const std::vector<glm::vec3> &nr,
			GLenum usage = GL_STATIC_DRAW) {
			addNormals(&nr[0], nr.size(), usage);
		};
		// Establece la normal que usarán *todos* los vértices
		void setNormal(const glm::vec3 &normal);

		/**
		Define las coordenadas de textura para dibujar la malla.
		\param tex_unit unidad de textura a la que hacen referencia las coordenadas
		de textura
		\param texcoords un vector con las coordenadas (puede ser float, glm::vec2, glm::vec3 o glm::vec4)
		\param usage el tipo de uso que se le dará al buffer object
		*/
		template<typename T>
		void addTexCoord(uint tex_unit, const std::vector<T> &texcoords, GLenum usage = GL_STATIC_DRAW) {
			static_assert(
				std::is_same<T, float>::value ||
				std::is_same<T, glm::vec2>::value ||
				std::is_same<T, glm::vec3>::value ||
				std::is_same<T, glm::vec4>::value, "Sólo texturas 1, 2, 3 y 4D");

			prepareNewVBO(TEX_COORD0 + tex_unit);

			if (texcoords.empty())
				return;

			createBufferAndCopy(TEX_COORD0 + tex_unit, sizeof(T) * texcoords.size(), usage, &texcoords[0]);
			glEnableVertexAttribArray(TEX_COORD0 + tex_unit);
			glVertexAttribPointer(TEX_COORD0 + tex_unit, sizeof(T) / sizeof(float), GL_FLOAT, GL_FALSE, 0, 0);
		}

		/**
		Define las coordenadas de textura para dibujar la malla.
		 \param tex_unit unidad de textura a la que hacen referencia las coordenadas
		  de textura
		\param t un vector de vec2 con las coordenadas
		\param n el número de coordenadas de textura
		\param usage el tipo de uso que se le dará al buffer object
		*/
		void addTexCoord(uint tex_unit, const glm::vec2 *t, size_t n,
			GLenum usage = GL_STATIC_DRAW);
		/** Define las coordenadas de textura para dibujar la malla.
		\param tex_unit unidad de textura a la que hacen referencia las coordenadas
		  de textura
		\param t un vector donde cada coordenada de textura está formada por dos
		  floats
		 \param n el número de coordenadas de textura
		 \param usage el tipo de uso que se le dará al buffer object
		 */
		void addTexCoord(uint tex_unit, const GLfloat *t, size_t n,
			GLenum usage = GL_STATIC_DRAW);
		void addTexCoord(uint tex_unit, const std::vector<glm::vec2> &t,
			GLenum usage = GL_STATIC_DRAW) {
			addTexCoord(tex_unit, &t[0], t.size(), usage);
		};
		// Define los colores para dibujar la malla. Cada color es de tipo vec4.
		// Recibe un vector de n colores.
		void addColors(const glm::vec4 *c, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define los colores para dibujar la malla. Cada color es de tipo vec4
		void addColors(const std::vector<glm::vec4> &c,
			GLenum usage = GL_STATIC_DRAW) {
			addColors(&c[0], c.size(), usage);
		};
		// Define los colores para dibujar la malla. Cada color está compuesto por
		// 'ncomponents' de floats consecutivos. Recibe un
		// vector de n colores.
		void addColors(const float *c, uint ncomponents, size_t nColors,
			GLenum usage = GL_STATIC_DRAW);
		// Establece el color que usarán *todos* los vértices
		void setColor(const glm::vec4 &color);
		// Define las binormales de cada vértice de la malla. Cada binormal es de tipo
		// vec3. Recibe un vector de n binormales.
		void addTangents(const glm::vec3 *t, size_t n, GLenum usage = GL_STATIC_DRAW);
		void addTangents(const std::vector<glm::vec3> &t,
			GLenum usage = GL_STATIC_DRAW) {
			addTangents(&t[0], t.size(), usage);
		};
		// Define las binormales de cada vértice de la malla. Cada binormal está
		// definida por 3 floats. Recibe un vector de n binormales.
		void addTangents(const float *t, size_t n, GLenum usage = GL_STATIC_DRAW);
		// Define la binormal que usarán *todos* los vértices
		void setTangent(const glm::vec3 &t);

		void setSkeleton(std::shared_ptr<Skeleton> skel);
		std::shared_ptr<Skeleton> getSkeleton() const;
		/**
		Inserta un atributo a los vértices.
		\param attribute_index índice del atributo (usar uno mayor o
		igual a _LAST_)
		\param type constante de OpenGL que define el tipo de los
		componentes del atributo (p.e., GL_FLOAT, GL_UNSIGNED_SHORT...)
		\param type_size tamaño de cada componente, en bytes (p.e.,
		sizeof(GLfloat)...)
		\param a puntero a los datos
		\param ncomponents número de componentes de cada atributo
		\param n cantidad de atributos proporcionados
		\param usage uso que se le va a dar al buffer (GL_STATIC_DRAW, GL_DYNAMIC_COPY...)
		*/
		void addAttribute(uint attribute_index, GLenum type, uint type_size,
			const void *a, uint ncomponents, size_t n,
			GLenum usage = GL_STATIC_DRAW);
		/**
		Inserta un atributo de tipo entero (sin normalizar ni convertir a float) a los vértices.
		\param attribute_index índice del atributo (usar uno mayor o igual a _LAST_)
		\param type constante de OpenGL que define el tipo de los
		componentes del atributo (p.e., GL_BYTE, GL_INT, GL_UNSIGNED_SHORT...)
		\param type_size tamaño de cada componente, en bytes (p.e.,
		sizeof(GLint)...)
		\param a puntero a los datos
		\param ncomponents número de componentes de cada atributo
		\param n cantidad de atributos proporcionados
		\param usage uso que se le va a dar al buffer (GL_STATIC_DRAW, GL_DYNAMIC_COPY...)
		*/
		void addIntegerAttribute(uint attribute_index, GLenum type, uint type_size,
			const void *a, uint ncomponents, size_t n,
			GLenum usage = GL_STATIC_DRAW);

		/**
		Asocia el contenido del buffer object proporcionado a un atributo del modelo.
		\param attribute_index índice del atributo
		\param bo Buffer Object con los datos a utilizar
		\param type constante de OpenGL que define el tipo de los
		componentes del atributo (p.e., GL_FLOAT, GL_UNSIGNED_SHORT...)
		\param ncomponents número de componentes de cada atributo
		*/
		void setAttribute(uint attribute_index, std::shared_ptr<BufferObject> bo,
			GLenum type, uint ncomponents);

		/**
		Asocia el contenido del buffer object proporcionado a un atributo entero del modelo.
		Los datos no se normalizan ni se convierten a float
		\param attribute_index índice del atributo
		\param bo Buffer Object con los datos a utilizar
		\param type constante de OpenGL que define el tipo de los
		componentes del atributo (p.e., GL_INT, GL_UNSIGNED_SHORT...)
		\param ncomponents número de componentes de cada atributo
		*/
		void setIntegerAttribute(uint attribute_index, std::shared_ptr<BufferObject> bo,
			GLenum type, uint ncomponents);

		// Establece un valor estático para el atributo de índice indicado. Todos los
		// vértices compartirán dicho valor para el atributo
		void setAttribute(uint attribute_index, const glm::vec3 &a);
		// Establece un valor estático para el atributo de índice indicado. Todos los
		// vértices compartirán dicho valor para el atributo
		void setAttribute(uint attribute_index, const glm::vec4 &a);

		/**
		Establece el material de la malla
		\param m material a asignar a la malla
		*/
		void setMaterial(std::shared_ptr<BaseMaterial> m);

		//! Devuelve el material asociado a la malla
		std::shared_ptr<BaseMaterial> getMaterial() const;

		/**
		Función que dibuja la malla
		*/
		void render();
		/**
		\return el número de vértices de la malla (cuidado! NO el número de
		 índices)
		*/
		size_t getNVertices() const { return n_vertices; };
		/**
		\return el número de normales de la malla (si hay normales, este número coincidirá
		con el número de vértices)
		*/
		size_t getNNormals() const;
		/**
		\return el número de índices de la malla
		*/
		size_t getNIndices() const { return n_indices; };
		/**
		\return el número de coordenadas de textura de la malla (si hay, este número coincidirá
		con el número de vértices)
		*/
		size_t getNTexCoord(int texUnit) const;
		/**
		  Añade una nueva orden de dibujado sobre los elementos del objeto Mesh
		  \param d El objeto que contiene la orden de dibujo. *No* liberes el
		  puntero
		  */
		void addDrawCommand(DrawCommand *d);
		/**
		  Elimina todas las órdenes de dibujado sobre el Mesh (no se dibujará
		  nada, aunque hayan vértices, colores, etc)
		  */
		void clearDrawCommands();

		/**
		Calcula las normales por vértice para simular una superficie suave. Sustituye
		las normales que hubieran definidas.
		\warning Llama a esta función una vez que la malla esté completamente
		configurada, es decir, con sus vértices, normales y draw commands.
		*/
		void computeSmoothNormals();
		/**
		Da acceso a los buffer objects que contienen la información de la malla
		\param which El buffer object deseado (VERTICES, NORMALS, etc)
		\return una referencia al buffer object
		*/
		std::shared_ptr<BufferObject> getBufferObject(BufferObjectType which);

		/**
		Da acceso a los buffer objects que contienen la información de la malla
		\param attribute el índice del atributo solicitado
		\return una referencia al buffer object
		*/
		std::shared_ptr<BufferObject> getBufferObject(int attribute);

		/**
		Establece el nombre de la malla
		*/
		void setName(const std::string &n);
		/**
		\return el nombre la malla (puede ser la cadena vacía)
		*/
		const std::string &getName() const;
		/**
		\return una cadena con una descripción de las características de la malla
		*/
		const std::string to_string() const;
		/**
		Devuelve la posición de los vértices de la malla
		\warning No abusar de estas funciones, puesto que tienen que traer la información
		desde la GPU
		*/
		std::vector<glm::vec3> getVertices() const;

		/**
		Devuelve las normales de los vértices de la malla
		\warning No abusar de estas funciones, puesto que tienen que traer la información
		desde la GPU
		*/
		std::vector<glm::vec3> getNormals() const;
		
		/**
		Devuelve las coordenadas de textura de los vértices de la malla
		\warning No abusar de estas funciones, puesto que tienen que traer la información
		desde la GPU
		*/
		std::vector<glm::vec2> getTexCoords(unsigned int texUnit = 0) const;

		/**
		Devuelve los índices de la malla
		\warning No abusar de estas funciones, puesto que tienen que traer la información
		desde la GPU
		*/
		std::vector<unsigned int> getIndices() const;

		const std::vector<DrawCommand *> &getDrawCommands() const { return drawCommands; }

		std::shared_ptr<UBOBones> getBones() const;

	protected:
		std::string name;
		std::vector<DrawCommand *> drawCommands;
		BoundingBox bb;
		BoundingSphere bs;
		VertexArrayObject vao;
		std::vector<std::shared_ptr<BufferObject>> vbos;
		GLenum indices_type;
		size_t n_indices, n_vertices;
		unsigned int n_components_per_vertex;
		struct StaticAttribute {
			StaticAttribute(GLint index, const glm::vec4 &val) {
				attrIndex = index;
				value = val;
			}
			GLint attrIndex;
			glm::vec4 value;
		};

		float epsilonSquared; // para determinar si dos vértices son iguales
		bool sameVertex(float *vs, uint ncomponents, uint a, uint b);
		std::vector<StaticAttribute> staticAttrValues;
		std::shared_ptr<BaseMaterial> material;
		std::shared_ptr<UBOBones> bones;
		std::shared_ptr<Skeleton> skeleton;

		/**
		  Almacena o actualiza un valor para el atributo indicado.
		  \param index índice del atributo
		  \param val Valor del atributo (valor estático, que se usará siempre que no
		  haya un array de atributos activo)
		  */
		void addStaticAttributeValue(GLint index, const glm::vec4 &val);
		/**
		  Elimina de la lista de atributos estáticos el atributo asociado al índice
		  dado.

		  \param index índice del atributo a eliminar de la lista (añadido con
		  Mesh::addStaticAttributeValue )
		  \returns true si el atributo estaba en la lista y se ha eliminado
		  */
		bool removeStaticAttributeValue(GLint index);

		/**
		  Prepara la entrada de un nuevo VBO:
		  1. Activa el VAO asociado a este Mesh
		  2. Libera el VBO anterior
		  3. Si había un valor de atributo estático asociado a este índice, lo
		  elimina y lanza un warning
		  */
		void prepareNewVBO(uint attribIndex);

		void createBufferAndCopy(uint attribIndex, size_t size, GLenum usage,
			const void *data);
		void addAttributeSetup(uint attribute_index, uint type_size,
			const void *a, uint ncomponents, size_t n, GLenum usage);
		void setAttributeSetup(uint attribute_index, std::shared_ptr<BufferObject> bo);

		/**
		Define los huesos que afectan a cada vértice de la malla (máximo 4 huesos por vértice)
		\param boneIds para cada vértice, se almacena el índice de cada hueso en una componente
			del uvec4
		\param usage el tipo de uso que se va a dar al atributo
		*/
		void addBoneIds(const std::vector<glm::uvec4> &boneIds, GLenum usage = GL_STATIC_DRAW);

		/**
		Define el peso de los huesos que afectan a cada vértice de la malla (máximo 4 huesos por vértice)
		\param boneWeights para cada vértice, se almacena el peso de cada hueso en una componente
		  del vec4 (deben sumar 1.0)
		\param usage el tipo de uso que se va a dar al atributo
		*/
		void addBoneWeights(const std::vector<glm::vec4> &boneWeights, GLenum usage = GL_STATIC_DRAW);

		void setBones(std::shared_ptr<UBOBones> b);


	};
};

#endif
