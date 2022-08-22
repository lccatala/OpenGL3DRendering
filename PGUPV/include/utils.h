
#ifndef _UTILS_H
#define _UTILS_H 2011

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <time.h>

#include "common.h"
#include "events.h"

#ifndef _DEBUG
#define CHECK_GL()
#define CHECK_GL2(msg)
#else
#define CHECK_GL() PGUPV::checkGLState(__FILE__, __LINE__)
#define CHECK_GL2(msg) PGUPV::checkGLState(__FILE__, __LINE__, msg)
#endif

namespace PGUPV {
	/////
	// FUNCIONES DE AYUDA SOBRE FICHEROS
	/////

	/**
	Comprueba que el fichero existe y se puede abrir para lectura
	\param filename Ruta de un fichero
	*/
	bool fileExists(const std::string &filename);


	bool dirExists(const std::string &path);

	// Devuelve la extensión del fichero, con el punto:
	// getExtension("pepe.TxT") -> ".TxT"
	std::string getExtension(const std::string &filename);

	//! El nombre de fichero indicado, sin extensión
	std::string removeExtension(const std::string &filename);

	bool loadTextFile(const std::string &filename, Strings &content);

	// Devuelve la fecha y hora de modificación del fichero (en segundos desde 1/1/1970)
	long long getFileModificationTime(const std::string &pathname);

	/**
	Dada una ruta completa de fichero, devuelve el directorio (la parte de la cadena
	antes de la última barra)
	\param filepath Ruta completa de un fichero
	*/
	std::string getDirectory(const std::string &filepath);

	/**
	Dada una ruta completa de fichero, devuelve el nombre de fichero (la parte de la
	cadena detrás de la última barra)
	\param filepath Ruta completa de un fichero
	\param includeExtension true si se quiere la extensión en el resultado
	*/
	std::string getFilenameFromPath(const std::string &filepath, bool includeExtension = true);

	/**
	Devuelve la lista de ficheros que se encuentran dentro del directorio indicado.
	\param path Directorio
	\param recursive Si es true, hará una búsqueda recursiva
	\return la lista de nombres de fichero
	*/
	std::vector<std::string> listFiles(const std::string &path, bool recursive);


	/**
	Devuelve la lista de directorios que se encuentran dentro del directorio indicado
	(no se incluye . y ..)
	\param path Directorio
	\return la lista de nombres de directorios
	*/
	std::vector<std::string> listDirs(const std::string &path);

	/**
	Devuelve la lista de ficheros que se encuentran dentro del directorio indicado y
	cuyo nombre coincide con alguno de los patrones indicados.
	\param path Directorio
	\param recursive Si es true, hará una búsqueda recursiva
	\param patterns lista de patrones a buscar (p.e., *.jpg, imagen??.bmp, log*.txt, etc.)
	\return la lista de nombres de fichero
	*/
	std::vector<std::string> listFiles(const std::string &path, bool recursive, const std::vector<std::string> &patterns);

	/**
	\return El directorio actual (acaba con la /)
	*/
	std::string getCurrentWorkingDir();

	/**
	Convierte de directorio relativo a absoluto (teniendo en cuenta el directorio actual)
	*/
	std::string relativeToAbsolute(const std::string &relative);

	/**
	Elimina las referencias al directorio actual y al padre de una ruta.
	P.e. removeDotAndDotDot("c:\pepe\juan\lucas\.\..\..\luis\..") -> "c:\pepe"
	*/
	std::string removeDotAndDotDot(const std::string &input);

	bool isAbsolutePath(const std::string &path);
	bool isRelativePath(const std::string &path);


	/**
	Borra el fichero indicado
	\return true si se ha podido borrar el fichero
	*/
	bool deleteFile(const std::string &filename);

	/**
	Cambia el directorio actual
	*/
	void changeCurrentDir(const std::string &dir);

	/**
	Crea el directorio indicado
	*/
	bool createDir(const std::string &dir);

	/**
	Cambia el nombre y/o mueve un fichero o directorio
	*/
	void renameOrMove(const std::string &from, const std::string &to);


	/**
	\return el número de apariciones de la cadena indicada
	*/
	void repetitionsInFile(const std::string &filename, const std::string &str);

	///////
	// FUNCIONES DE AYUDA SOBRE GL
	///////

	std::string decodeGLError(GLenum err);

	void checkGLState(std::string filename, int line, std::string msg = "");

	/**
	Indica si la extensión de OpenGL está disponible. No usar las funciones de GLEW,
	pues parece que están rotas (ver
	http://stackoverflow.com/questions/18447881/opengl-anisotropic-filtering-support-contradictory-check-results)
	\param name Nombre de la extensión (por ejemplo,
	GL_EXT_texture_filter_anisotropic)
	\return true si la extensión está disponible
	*/
	bool isGLExtensionAvailable(const std::string &name);

	/**
	Imprime en el flujo indicado todas las extensiones disponibles en la máquina

	\param o el flujo donde escribir los nombres
	*/
	void printAvailableGLExtensions(std::ostream &o);

	/**
	\return el número de extensiones disponibles en el sistema
	*/
	uint getNumAvailableExtensions();

	/**
	\return el fabricante de la tarjeta gráfica
	\warning Sólo se puede invocar esta función si existe un contexto OpenGL (es
	decir, después de llamar a App::initApp)
	*/
	enum class Manufacturer { UNKNOWN, NVIDIA, AMD, INTEL };
	Manufacturer getManufacturer();


	///////
	// FUNCIONES DE AYUDA MATEMATICAS
	///////


	// Devuelve el cuadrado de la distancia entre a y b
	float distSquare(glm::vec3 a, glm::vec3 b);

	// Aplica la matriz de transformación a los vértices indicados, y devuelve un
	// nuevo vector con el resultado
	// Recuerda liberarlo cuando ya no te haga falta!!!
	glm::vec3 *apply(const glm::mat3 &m, const glm::vec3 *v, uint n);
	glm::vec4 *apply(const glm::mat4 &m, const glm::vec4 *v, uint n);
	glm::vec3 *apply(const glm::mat4 &m, const glm::vec3 *v, uint n);
	glm::vec2 *apply(const glm::mat4 &m, const glm::vec2 *v, uint n);

	// Sirve para floats y doubles
	template <typename T>
	T *apply(const glm::mat4 &m, const T *v, uint ncomponents, uint n) {
		// Crea una copia de los vértices, aplicando la transformación indicada
		T *vt = new T[ncomponents * n];
		for (uint i = 0; i < n; i++) {
			glm::vec4 p;
			for (uint j = 0; j < ncomponents; j++) {
				p[j] = v[i * ncomponents + j];
			}
			glm::vec4 tp = m * p;
			for (uint j = 0; j < ncomponents; j++) {
				vt[i * ncomponents + j] = tp[j];
			}
		}
		return vt;
	};

	// float *apply(glm::mat4 m, const float *v, uint ncomponents, uint n);
	float *apply(const glm::mat3 &m, const float *v, uint n);

	/**
	Convierte de vec2, vec3 y vec4 a vec3.
	GLSL lo soporta automáticamente, pero parece que glm no
	*/

	inline glm::vec3 toVec3(const glm::vec2 &v) {
		return glm::vec3(v.x, v.y, 0.0f);
	};

	inline glm::vec3 toVec3(const glm::vec3 &v) {
		return v;
	}
	inline glm::vec3 toVec3(const glm::vec4 &v) {
		return glm::vec3(v.x, v.y, v.z);
	}

	template <typename T, uint Length> float max(const T &v) {
		float largest = v[0];
		for (uint i = 1; i < v.Length; i++) {
			if (v[i] > largest)
				largest = v[i];
		}
		return largest;
	}

	template <typename T, uint Rows, uint Columns> float max(const T &m) {
		float largest = FLT_MIN;

		for (uint c = 0; c < Columns; c++) {
			for (uint r = 0; r < Rows; r++) {
				if (m[c][r] > largest)
					largest = m[c][r];
			}
		}
		return largest;
	}

	/**
	\return true si la matriz es la identidad
	*/
	template <typename M>
	bool isIdentity(const M &m, float epsilon = 0.00001f) {
		for (typename M::length_type i = 0; i < m.length(); i++) {
			for (typename M::length_type j = 0; j < m.length(); j++) {
				if (i != j && fabs(m[i][j]) > epsilon)
					return false;
				else if (fabs(m[i][j] - 1.0f) > epsilon)
					return false;
			}
		}
		return true;
	}

	// http://www.realtimerendering.com/resources/GraphicsGems/gemsii/unmatrix.c
	bool extractTRSfromMatrix(const glm::mat4 mat, glm::vec3 &trans, glm::mat4 &rot, glm::vec3 &scale);

	// Remove scaling from matrix
	glm::mat4 removeScaling(const glm::mat4 &m);


	///////
	// FUNCIONES DE AYUDA SOBRE FECHAS
	///////

	// Devuelve la fecha y hora actuales en el formato Mon Oct 7 10:30:23 2013
	std::string getCurrentDateTimeString();
	// Devuelve la fecha y hora actuales en el formato 20131023-172213000
	// (yyyymmdd-hhmmssnnn), donde n son milisegundos
	std::string getTimeStamp();

	// Devuelve la fecha y hora actuales (en segundos desde 1/1/1970)
	long long getCurrentTime();


	///////
	// FUNCIONES DE AYUDA ESPECIFICAS DE PGUPV
	///////

	// Construye un nombre de fichero de la forma <base>xxxxxxxx.<ext>, donde
	// xxxxxxxx es number, con 8 posiciones decimales
	std::string buildFrameName(std::string base, const ulong number,
		std::string extension = "png");

	/**
	\return el sistema operativo para el que se compiló la librería
	*/
	enum class Platform { UNKNOWN, WIN, LINUX, MAC };
	constexpr Platform getPlatform() {
#ifdef _WIN32	
		return Platform::WIN;
#elif __APPLE__
		return Platform::MAC;
#elif __linux__
		return Platform::LINUX;
#else
		return Platform::UNKNOWN;
#endif
	}


	////////
	// FUNCIONES DE AYUDA SOBRE CADENAS
	////////
	// Devuelve una representación del número en hexadecimal, en forma de 0xYYY
	std::string hexString(ulong n);

	/**
	Quita los espacios del principio y del final de la cadena.
	*/
	void trim(std::string &s);

	/**
	Devuelve una copia de la cadena en minúsculas
	*/
	std::string to_lower(const std::string &s);

	/**
	Devuelve si la cadena s empieza por b
	*/
	bool starts_with(const std::string &s, const std::string &b);

	/**
	Devuelve si la cadena s acaba por b, ignorando minúsculas/mayúsculas
	*/
	bool ends_with(const std::string &s, const std::string &b);

	/**
	\return true si la cadena s contiene la cadena b
	http://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find
	*/
	bool containsIgnoreCase(const std::string &s, const std::string &b);

	/**
	Reemplaza todas las líneas de org que coincidan con un índice del mapa por el
	conjunto de cadenas asociado al elemento del mapa.
	\param transTable Tabla de traducciones. Las variables a expandir empiezan por $ y no tienen espacios
	\param org Texto a expandir
	*/
	Strings expandText(const std::map<std::string, Strings> &transTable,
		const Strings &org);


	std::string to_string(float f, uint ndecimals = 3);
	std::string to_string(const glm::vec4 &v, uint ndecimals = 3);
	std::string to_string(const glm::vec3 &v, uint ndecimals = 3);
	std::string to_string(const glm::vec2 &v, uint ndecimals = 3);

	/* Convierte la cadena dada en un vector de glm

	Los formatos esperados son (con paréntesis):

	(x, y) devuelve un vec2
	(x, y, z) devuelve un vec3
	(x, y, z, w) devuelve un vec4

	Para usarlo, hacer, por ejemplo:

	glm::vec3 v = PGUPV::to_vec<glm::vec3>("(1.0, 0.0, 1.0)");
	*/
	template<typename T>
	T to_vec(const std::string &str) {
		std::istringstream ss(str);
		T error(std::numeric_limits<typename T::value_type>::quiet_NaN()), result;

		int parent;
		parent = ss.get();
		if (parent != '(') return error;
		int comma;
		ss >> result[0];
		if (!ss) return error;
		for (size_t i = 1; i < sizeof(T) / sizeof(typename T::value_type); i++) {
			comma = ss.get();
			if (comma != ',') return error;
			ss >> result[static_cast<typename T::length_type>(i)];
			if (!ss) return error;
		}
		parent = ss.get();
		if (parent != ')') return error;
		return result;
	}

	std::string to_string(const glm::ivec4 &v);
	std::string to_string(const glm::ivec3 &v);
	std::string to_string(const glm::ivec2 &v);

	/** Devuelve la matriz representada como una cadena con saltos de línea entre
	 * cada fila */
	std::string to_string(const glm::mat3 &m, uint ndecimals = 3);
	std::string to_string(const glm::mat4 &m, uint ndecimals = 3);

	int strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource);

	/**
	Escribe en el flujo un volcado hexadecimal del buffer.
	\param buffer puntero al buffer que contiene los datos
	\param size tamaño en bytes del buffer anterior
	\param output flujo donde escribir el volcado
	*/
	void hexdump(uchar *buffer, uint size, std::ostream &output);

	/**
	 Convierte una cadena utf8 a latin1
	 \param utf8 la cadena codificada en utf8 a convertir
	 */
	std::string utf8ToLatin1(std::string utf8);

	std::string to_string(HatPosition pos);

	///////
	// FUNCIONES DE AYUDA DEL SO
	///////

	/**
	Ejecuta un programa con argumentos. Esta llamada espera a que se acabe el programa lanzado
	*/
	int execute(std::string filename, std::vector<std::string> args = std::vector<std::string>());

	////////
	// FUNCIONES DE AYUDA SOBRE C++
	////////

	// http://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
	template <typename E>
	constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
		return static_cast<typename std::underlying_type<E>::type>(e);
	}

	template< typename E, typename T>
	constexpr inline typename std::enable_if< std::is_enum<E>::value && std::is_integral<T>::value, E>::type
		to_enum(T value) noexcept
	{
		return static_cast<E>(value);
	}

	/**
	\return El número de elementos del array, como una constante en tiempo de compilación
	*/
	// Effective C++, pp. 16
	template<typename T, std::size_t N>
	constexpr std::size_t arraySize(T(&)[N]) noexcept { return N; }

	// http://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)


	template <typename K, typename V>
	const std::vector<K> getKeysFromMap(const std::map<K, V> &m) {
		std::vector<K> keys;
		keys.reserve(m.size());
		std::transform(
			m.begin(), m.end(),
			std::back_inserter(keys),
			[](const typename std::map<K, V>::value_type &pair) {return pair.first; });
		return keys;
	}

	template <typename K, typename V>
	const std::vector<V> getValuesFromMap(const std::map<K, V> &m) {
		std::vector<V> vals;
		vals.reserve(m.size());
		std::transform(
			m.begin(), m.end(),
			std::back_inserter(vals),
			[](const typename std::map<K, V>::value_type &pair) {return pair.second; });
		return vals;
	}


	}; // namespace PGUPV

#endif
