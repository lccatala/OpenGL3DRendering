#ifndef _SHADERS_H
#define _SHADERS_H 2011

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iostream>

#include <GL/glew.h>

#include "utils.h"
#include "uniformBufferObject.h"
#include "shader.h"

namespace PGUPV {

	class Program;

	struct Attribute {
		unsigned int loc;
		std::string name;
	};

	typedef std::vector<unsigned int> Uints;

	/**
	Clase contenedora de información sobre un uniform activo en el programa.
	*/
	struct UniformInfo {
		/**
		Constructor
		\param name nombre del uniform
		\param offset desplazamiento desde el inicio del buffer
		\param size tamaño del uniform
		\param type constante de GL con la definición del tipo (GL_FLOAT,
		GL_FLOAT_VEC4, etc)
		*/
		UniformInfo(const std::string &name, GLint offset, GLint size, GLint type)
			: name(name), offset(offset), size(size), type(type) {};
		std::string name;
		GLint offset, size, type;
		std::string toString() const;
		std::string toString(void *bo) const;
	};

	/**
	Clase contenedora de los uniforms de un bloque
	*/
	struct UniformInfoBlock {
		std::string name;
		GLint binding;
		GLint blockSize;
		std::vector<UniformInfo> uniforms;
	};

	/**
	Clase contenedora de bloques de uniforms. El primero se corresponde al bloque
	por defecto.
	*/
	struct UniformInfoBlocks {
		uint totalActiveUniforms;
		std::vector<UniformInfo> defaultBlockUniforms;
		std::vector<UniformInfoBlock> blocks;
	};

	/**
	\class Program

	Esta clase representa a un programa ejecutable en la GPU, que está compuesto por
	varios
	shaders.

	*/

	class Program {
	public:
		Program();
		~Program();
		/**
		Intenta cargar los ficheros que componen el programa, añadiendo a 'name' las
		extensiones por defecto definidas en la clase Shader
		\return El número de ficheros cargados.
		*/
		int loadFiles(const std::string &name);

		/**
		Intentar cargar los ficheros indicados en el parámetro. Deberían tener las extensiones
		por defecto.
		\return El número de ficheros cargados (lanza una excepción si falla alguno)
		*/
		int loadFiles(const std::vector<std::string> &files);

		/**
		Carga las cadenas que componen el programa
		\return El número de shaders creados.
		*/
		int loadStrings(
			const std::vector<std::string> &vertexShader,
			const std::vector<std::string> &fragmentShader = std::vector<std::string>(),
			const std::vector<std::string> &geometryShader = std::vector<std::string>(),
			const std::vector<std::string> &tessCtrlShader = std::vector<std::string>(),
			const std::vector<std::string> &tessEvalShader = std::vector<std::string>());

		/**
		Carga desde memoría un shader de computación
		*/
		void loadComputeStrings(const std::vector<std::string> &computeShader);

		/**
		Carga, compila, crea un programa, adjunta los shaders al programa y enlaza el
		programa.
		Antes se han debido declarar las posiciones de los atributos con
		\sa{Program::addAttributeLocation}
		\return true si el programa se ha creado correctamente
		*/
		bool compile();
		/**
		Libera todos los recursos asociados a este shader (memoria, shaders
		compilados, etc)
		*/
		void release();

		/**
		Indica la localización de los atributos de un shader.
		\warning Llamar antes de compilar el shader
		\param loc Posición del atributo (entre 0 y 15)
		\param name Nombre de la variable in del shader de vértice que recibirá el
		atributo
		*/
		void addAttributeLocation(unsigned int loc, const std::string &name);

		/**
	  Devuelve la localización del uniform indicado en el programa. Esta localización
	  sólo cambia al enlazar el programa
	  \param uniform Nombre del uniform cuya localización se desea obtener
	  \return Localización del uniform (para usarla en glUniform*)
	  */
		int getUniformLocation(const std::string &uniform);

		/**
		Conecta el buffer object con el programa. Esta función pide al objeto bo el
		nombre del bloque y su definición durante la compilación del shader y realiza
		las conexiones automáticamente. Se usa normalmente, para vincular objetos de
		las clases UBOMaterial, UBOLightSources o GLMatrices.
		\warning Llamar a esta función antes de compilar el programa
		\param bo un puntero inteligente al objeto UniformBufferObject
		\param bindingPoint índice del punto de vinculación GL_UNIFORM_BUFFER donde
		conectar el buffer object.
		*/
		void connectUniformBlock(std::shared_ptr<UniformBufferObject> bo,
			GLuint bindingPoint);

		/**
		Conecta el buffer object con el shader. Asume que la definición del bloque
		uniforme ya está en el código fuente del shader (o se incluirá en el momento
		de la compilación con la llamada addReplaceString). Usa esta función para
		vincular un bloque que hayas definido tú.
		\warning Llamar a esta función antes de compilar el programa
		\param blockName Nombre de la variable uniform block en el shader
		\param bo un puntero inteligente al objeto UniformBufferObject
		\param bindingPoint índice del punto de vinculación GL_UNIFORM_BUFFER donde
		conectar el buffer object.
		*/
		void connectUniformBlock(const std::string &blockName,
			std::shared_ptr<UniformBufferObject> bo,
			GLuint bindingPoint);

		/**
		Conecta el bloque definido en el código al punto de vinculación dado. Por
		ejemplo: layout (std140) uniform Material {...
		Luego, en tiempo de ejecución habrá que conectar al mismo punto de vinculación
		un BO con la información que recibirá el shader al acceder a los miembros del
		bloque. Dicha conexión se puede hacer con gl_uniform_buffer.bindBufferBase(...
		\warning Llama a esta función después de haber compilado el shader
		\param blockName nombre del bloque definido en el shader (Material en el
		ejemplo anterior)
		\param bindingPoint identificador del punto de vinculación
		\deprecated A partir de GL 4.20, desde el shader se puede dar directamente el 
		   punto de vinculación con layout (std140,binding=XX) uniform ...
		*/
		bool bindBlockToBindingPoint(const std::string &blockName,
			GLuint bindingPoint);

		/**
		Instala el shader en la GPU.
		\param reconnectUBOs si true (por defecto) reconecta todos los UBO que se
		declararon con connectUniformBlock. Si false, no reconecta los UBO (así, podrás
		cambiar de UBO llamando a gl_uniform_buffer.bind(ubo);
		\return el último programa que llamó a este método
		*/
		Program *use(bool reconnectUBOs = true);

		/**
		Desinstala el programa de la GPU, dejando como programa actual el 0
		\warning Sólo en raras ocasiones necesitas llamar a esta función. Puedes dejar
		un programa instalado hasta que necesites el siguiente. Al instalar un programa
		(con Program::use), se deinstala el anterior
		*/
		void unUse();

		/**
		Devuelve el tamaño del bloque de uniforms, según el driver de OpenGL.
		El shader debe haberse compilado correctamente (y enlazado en un programa)
		\param blockName nombre de la variable uniform block
		\return Tamaño en bytes
		*/
		uint getUniformBlockSize(const std::string &blockName);

		/**
		Devuelve el desplazamiento desde el comienzo del BO donde comienza la memoria
		del miembro del bloque de uniformes indicado
		\return el desplazamiento en bytes desde el comienzo del BO, o -1 si no se
		encuentra el bloque
		o el miembro, o si el shader no está enlazado.
		*/
		int getUniformBlockMemberOffset(const std::string &member);

		int getUniformBlockMemberOffset(const std::string &blockName,
			const std::string &member);

		/**
		Devuelve información sobre el programa compilado
		\param pname Una de las constantes admitidas por glGetProgramiv
		(http://www.opengl.org/sdk/docs/man3/xhtml/glGetProgram.xml)
		\return Un entero con el valor pedido
		*/
		int getProgramInfo(GLenum pname) const;

		/**
		Devuelve el nombre del uniform activo con el índice indicado
		\param idx índice dle uniform cuyo nombre se desea obtener
		\param name variable de salida donde se escribirá el nombre
		\return 0 si tiene éxito, -1 en caso de 


		*/
		int getActiveUniformName(uint idx, std::string &name) const;

		/**
		Imprime en el flujo indicado un montón de información sobre OpenGL en esta
		máquina
		\param os flujo de salida donde escribir la información
		\param verbose escribe aún más información, como la lista de extensiones
		*/
		static void printOpenGLInfo(std::ostream &os, bool verbose = false);

		/**
		Imprime en el flujo indicado los objetos textura que están vinculados a
		cada unidad de textura.
		*/
		static void printBoundTextures(std::ostream &os);
		/**
		Imprime en el flujo indicado las texturas vinculadas a las unidades de imagen
		*/
		static void printBoundImages(std::ostream &os);
		/**
		Imprime en el flujo indicado los shader storage buffer vinculados
		*/
		static void printBoundShaderStorageBuffers(std::ostream &os);
		/**
		Imprime en el flujo indicado un montón de información sobre el programa (una
		vez enlazado)
		*/
		void printInfo(std::ostream &os = std::cout) const;

		/**
		\return el identificador del programa
		*/
		GLuint getId() const { return programId; };

		/**
		\return el nombre de fichero que contiene el shader del tipo indicado
		*/
		std::string getFileName(Shader::ShaderType type) const;
		/**
		Establece una cadena que será reemplaza por otra justo antes de
		compilar o mostrar el shader. Se puede usar para firmar shaders,
		añadir macros, etc.

		\param replaceThis: cadena a reemplazar
		\param byThis: nueva cadena
		*/
		void replaceString(const std::string &replaceThis, const std::string &byThis);

		/**
		Establece una cadena que será reemplaza por varias cadenas justo antes de
		compilar o mostrar el
		shader. Se puede usar para firmar shaders, añadir macros, etc.

		\param replaceThis: cadena a reemplazar
		\param byThis: vector de cadenas que sustituirán la aparición del replaceThis
		en el shader
		*/
		void replaceString(const std::string &replaceThis, const Strings &byThis);

		/**
		Devuelve si hay un shader cargado del tipo indicado
		\param type uno de las constantes de tipo ShaderType
		*/
		bool hasShaderType(Shader::ShaderType type) const;

		/**
		Imprime el código de todos los shaders que componen el programa
		\param os el flujo de salida donde imprimir el código fuente
		*/
		void printSrcs(std::ostream &os) const;

		/**
		Añade el shader proporcionado al programa. El shader debe estar listo para su
		compilación
		(no se expandirán cadenas ni se preprocesará el código fuente).
		Si ya existiera un shader con el mismo tipo, lo reemplazaría.
		\param shader Shader a incluir en el programa
		*/
		void addShader(std::shared_ptr<Shader> shader);
		/**
		Desvincula el shader del tipo indicado del programa. El shader puede seguir
		usándose en otros
		programas. No falla si el programa no tiene ningún shader del tipo indicado.
		*/
		void removeShader(Shader::ShaderType type);

		/**
		Devuelve una referencia al shader del tipo indicado
		\param type Tipo del shader a devolver
		*/
		std::shared_ptr<Shader> getShader(Shader::ShaderType type) const;

		/**
		Devuelve la cantidad de shaders que componen el programa
		*/
		size_t getNumShaders() const { return shaders.size(); };

		/**
		Devuelve los uniforms activos del programa
		*/
		UniformInfoBlocks getActiveUniforms() const;

		/**
		Establece las variables de salida que se almacenarán en el buffer correspondiente
		si está activo el Transform Feedback. (http://docs.gl/gl4/glTransformFeedbackVaryings)
		\param vars lista de nombres de variables y, quizá, variables virtuales (p.e.,
		gl_SkipComponents1...)
		\param interleaved si true, todas las variables se almacenarán en el mismo buffer.
		Si no, se almacenarán en buffers distintos.
		\warning Esta acción requiere recompilar el programa
		*/
		void setTransformFeedbackVaryings(std::vector<std::string> vars, bool interleaved);

		//// Grupo de funciones para trabajar con subrutinas

		/**
		 Establece qué rutina se asociará al uniform dado cada vez que se instale este
		 shader.
		 ¡Cuidado! Antes de instalar el shader por primera vez, tendrás que decidir
		 qué subrutina se asocia a cada uniform, para todos los uniforms en todos los
		 tipos de shader
		 \param type Tipo de shader donde se encuentra la subrutina
		 \param uniformRoutine nombre la variable uniform que define la rutina a
		 utilizar
		 \param routineName nombre de la rutina a usar
		 */
		void setRoutine(Shader::ShaderType type, std::string uniformRoutine,
			std::string routineName);

		/**
		 \return La fecha de modificación del shader más reciente (se puede usar para comprobar
			si se ha modificado algún shader)
		 */
		long long getModificationTime();
	private:
		// Prohibir la copia
		Program(const Program &);
		Program &operator=(Program);

		// Devuelve la localización del uniform de tipo subrutina indicado en el
		// shader de tipo indicado
		uint getSubroutineUniformLocation(Shader::ShaderType type, std::string name);
		// Devuelve el índice de la subrutina indicada
		GLuint getSubroutineIndex(Shader::ShaderType type, std::string name);
		// Las rutinas asociadas a cada shader
		std::vector<GLuint> subrutinas[Shader::NUM_SHADER_TYPES];
		// Establece todos los enlaces uniforms de rutina con sus rutinas
		void refreshRoutineUniforms();

		bool bindUBOs();
		void bindAttribs();
		bool linkProgram(const Uints &shids, std::ostream &error_output);
		std::map<Shader::ShaderType, std::shared_ptr<Shader>> shaders;
		std::vector<struct Attribute> attribs;
		GLuint programId;
		std::map<std::string, Strings> subStrings;
		struct PendingConnection {
			std::string blockName;
			std::shared_ptr<UniformBufferObject> bo;
			GLuint bindingPoint;
		};
		std::vector<PendingConnection> pendingConnections;
		std::vector<std::string> transformVaryings;
		bool transformInterleaved;

		static Program *prevProgram;
	};

} // namespace

#endif
