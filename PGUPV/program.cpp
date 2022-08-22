
#include <fstream>

#include <sstream>

#include <iomanip>
#include <algorithm>
#include <gsl/gsl>

#include <assert.h>

#include "app.h"
#include "program.h"
#include "utils.h"
#include "common.h"
#include "log.h"
#include "indexedBindingPoint.h"
#include "glslInfo.h"
#include "material.h"

using std::cout;
using std::cerr;
using std::string;
using std::endl;
using std::setw;

using PGUPV::Program;
using PGUPV::BufferObject;
using PGUPV::UniformBufferObject;
using PGUPV::ShaderLibrary;
using PGUPV::Shader;
using PGUPV::UniformInfo;
using PGUPV::UniformInfoBlocks;

Program *Program::prevProgram = nullptr;

void declareVarTU(const std::string &base, uint texUnitBase, Program &m) {
	m.replaceString(base, std::to_string(texUnitBase));
	for (int i = 2; i <= 4; i++)
		m.replaceString(base + std::to_string(i), std::to_string(texUnitBase + i - 1));
}

Program::Program() : programId(0) {
	App::getInstance().getShaderLibrary().add(this);

	declareVarTU("$TEXDIFF", PGUPV::Material::DIFFUSE_TUNIT, *this);
	declareVarTU("$TEXSPEC", PGUPV::Material::SPECULAR_TUNIT, *this);
	declareVarTU("$TEXNORM", PGUPV::Material::NORMALMAP_TUNIT, *this);
	declareVarTU("$TEXHEIGHT", PGUPV::Material::HEIGHTMAP_TUNIT, *this);
	declareVarTU("$TEXOPACITY", PGUPV::Material::OPACITYMAP_TUNIT, *this);
}

Program::~Program() {
	App::getInstance().getShaderLibrary().remove(this);
	release();
};

void Program::release() {
	if (programId) {
		glDeleteProgram(programId);
		programId = 0;
	}
}

int Program::loadFiles(const std::string &name) {
  if (programId) {
    ERRT("El programa ya está compilado. Deberías crear un Programa nuevo");
  }

  int count = 0;
	for (int i = 0; i < Shader::NUM_SHADER_TYPES; i++) {
		std::string f =
			name + Shader::getDefaultShaderExtension((Shader::ShaderType)i);
		if (fileExists(f)) {
			addShader(Shader::loadFromFile(f, (Shader::ShaderType)i, subStrings));
			count++;
		}
	}
	if (count == 0) {
		ERRT("No se han encontrado los ficheros con los shaders " + name + "{" +
			Shader::getDefaultShaderExtension(Shader::VERTEX_SHADER) + ", " +
			Shader::getDefaultShaderExtension(Shader::FRAGMENT_SHADER) + "...}");
	}
#ifndef __APPLE__
	if (shaders.find(Shader::COMPUTE_SHADER) != shaders.end() &&
		shaders.size() > 1) {
		ERRT("Un shader de computación no puede enlazarse con ningún otro tipo de "
			"shader");
	}
#endif
	return count;
}

int Program::loadFiles(const std::vector<std::string> &files) {

  if (programId) {
    ERRT("El programa ya está compilado. Deberías crear un Programa nuevo");
  }

	int count = 0;
	for (auto name : files) {
		auto s = Shader::loadFromFile(name, Shader::CHECK_EXTENSION, subStrings);
		assert(s.get());
		addShader(s);
		count++;
	}
#ifndef __APPLE__
	if (shaders.find(Shader::COMPUTE_SHADER) != shaders.end() &&
		shaders.size() > 1) {
		ERRT("Un shader de computación no puede enlazarse con ningún otro tipo de "
			"shader");
	}
#endif
	return count;
}

int Program::loadStrings(
	const std::vector<std::string> &vertexShader,
	const std::vector<std::string> &fragmentShader,
	const std::vector<std::string> &geometryShader,
	const std::vector<std::string> &tessCtrlShader,
	const std::vector<std::string> &tessEvalShader) {

	int count = 0;
	if (!vertexShader.empty()) {
		addShader(Shader::loadFromMemory(vertexShader, Shader::VERTEX_SHADER, subStrings));
		count++;
	}
	if (!fragmentShader.empty()) {
		addShader(Shader::loadFromMemory(fragmentShader, Shader::FRAGMENT_SHADER, subStrings));
		count++;
	}
	if (!geometryShader.empty()) {
		addShader(Shader::loadFromMemory(geometryShader, Shader::GEOMETRY_SHADER, subStrings));
		count++;
	}
	if (!tessCtrlShader.empty()) {
		addShader(Shader::loadFromMemory(tessCtrlShader, Shader::TESS_CONTROL_SHADER, subStrings));
		count++;
	}
	if (!tessEvalShader.empty()) {
		addShader(Shader::loadFromMemory(tessEvalShader, Shader::TESS_EVALUATION_SHADER, subStrings));
		count++;
	}

	if (count == 0)
		ERRT("No se ha suministrado el código fuente de los shaders");

	return count;
}

void Program::loadComputeStrings(const std::vector<std::string> &computeShader) {
#ifndef __APPLE__
	if (!computeShader.empty())
		addShader(Shader::loadFromMemory(computeShader, Shader::COMPUTE_SHADER, subStrings));
	else
		ERRT("No se ha suministrado el código fuente del shader");
#else
	ERRT("Mac OS no soporta los shaders de computación");
#endif
}


std::shared_ptr<Shader> Program::getShader(Shader::ShaderType type) const {
	if (type < Shader::VERTEX_SHADER || type >= Shader::NUM_SHADER_TYPES)
		ERRT("Tipo de shader desconocido");

	std::map<Shader::ShaderType, std::shared_ptr<Shader>>::const_iterator i =
		shaders.find(type);
	if (i != shaders.end())
		return i->second;
	return std::shared_ptr<Shader>();
}

// Devuelve el nombre de fichero del shader indicado
std::string Program::getFileName(Shader::ShaderType type) const {
	std::shared_ptr<Shader> shader = getShader(type);
	if (shader.get() != NULL) {
		return shader->getFilename();
	}
	ERRT("El programa no tiene un shader del tipo pedido");
}


bool Program::compile() {
	Uints tolink;
	std::stringstream compilationResult;

	CHECK_GL();

	if (shaders.empty())
		ERRT("No hay nada que compilar");

	if (programId != 0) {
		glDeleteProgram(programId);
		programId = 0;
	}

	for (std::map<Shader::ShaderType, std::shared_ptr<Shader>>::iterator i =
		shaders.begin();
		i != shaders.end(); ++i) {
		GLuint sid = (i->second)->compile();
		if (sid == 0) {
			compilationResult << (i->second)->getErrorMessage() << std::endl;
			(i->second)->printSrc(compilationResult);
			// No seguimos compilando
			break;
		}
		else {
			tolink.push_back(sid);
		}
	}

	if (tolink.size() == shaders.size())
		linkProgram(tolink, compilationResult);

	if (!programId) {
		// Ha fallado algo...
		if (tolink.size() == shaders.size()) {
			// Ha fallado el enlace: imprimir todos los shaders
			printSrcs(compilationResult);
		}
		ERRT(compilationResult.str());
	}

	bindUBOs();
	return true;
}

int Program::getUniformLocation(const std::string &uniform) {
	if (programId == 0)
		ERRT("No se puede pedir la posición de un uniform si el shader no está "
			"enlazado");

	CHECK_GL();

	GLint loc = glGetUniformLocation(programId, uniform.c_str());
	if (loc == -1)
		ERR("¡Cuidado! No se encuentra la variable uniform " + uniform +
			". Asegúrate de que el programa donde"
			" se encuentra el uniform está enlazado y la variable no ha sido "
			"eliminada por el compilador");
	return loc;
}

void Program::bindAttribs() {
	for (uint i = 0; i < attribs.size(); i++)
		glBindAttribLocation(programId, attribs[i].loc, attribs[i].name.c_str());
}

void Program::addAttributeLocation(unsigned int loc, const std::string &name) {
	PGUPV::Attribute at;

	at.loc = loc;
	at.name = name;
	attribs.push_back(at);
}

/*

Enlaza un programa compuesto por uno o más shaders.
Devuelve el identificador de programa, o 0 si se produce algún error.
Parámetros de entrada:
-un std::vector de GLuint, con los identificadores de shader a enlazar.

*/
bool Program::linkProgram(const PGUPV::Uints &shids,
	std::ostream &error_output) {
	GLint linked;

	programId = glCreateProgram();
	for (unsigned int i = 0; i < shids.size(); i++)
		glAttachShader(programId, shids[i]);

	bindAttribs();

	if (!transformVaryings.empty()) {
		const char **vars = new const char *[transformVaryings.size()];
		for (unsigned int i = 0; i < transformVaryings.size(); i++) {
			vars[i] = transformVaryings[i].c_str();
		}
		glTransformFeedbackVaryings(programId, gsl::narrow<GLsizei>(transformVaryings.size()), vars,
			transformInterleaved ? GL_INTERLEAVED_ATTRIBS : GL_SEPARATE_ATTRIBS);
		delete[] vars;
	}

	glLinkProgram(programId);
	CHECK_GL();

	glGetProgramiv(programId, GL_LINK_STATUS, &linked);

	if (linked == GL_FALSE) {
		GLint length;
		GLchar *log;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
		log = new GLchar[length];
		glGetProgramInfoLog(programId, length, &length, log);
		error_output << "Error enlazando el programa:" << endl;
		error_output << log << endl;
		delete[] log;
		glDeleteProgram(programId);
		programId = 0;
		return false;
	}
	return true;
}

Program *Program::use(bool reconnectUBOs) {
  // Si es el mismo programa que hay instalado y no hay que recompilarlo, 
  // terminar
  if (this == prevProgram && programId != 0)
    return this;

	if (!programId) {
		compile();
	}
	// Otro programa puede haber cambiado los UBO vinculados. Revincular
	if (reconnectUBOs && !bindUBOs())
		ERRT("Error intentando vincular los bloques uniform");

	if (programId)
		glUseProgram(programId);
	else
		ERRT("Intentando activar un programa inexistente");

	refreshRoutineUniforms();
	Program *prev = prevProgram;
	prevProgram = this;
	return prev;
}

void Program::unUse()
{
	prevProgram = nullptr;
	glUseProgram(0);
}

bool Program::bindBlockToBindingPoint(const std::string &blockName,
	GLuint bindingPoint) {
	if (programId == 0)
		ERRT("Antes de llamar a Program::bindBlockToBindingPoint debes compilar el programa");

	GLuint idx = glGetUniformBlockIndex(programId, blockName.c_str());
	if (idx == GL_INVALID_INDEX)
		return false;

	glUniformBlockBinding(programId, idx, bindingPoint);

	return true;
}

void Program::connectUniformBlock(const std::string &blockName,
	std::shared_ptr<UniformBufferObject> bo,
	GLuint bindingPoint) {
	PendingConnection pc;
	pc.blockName = blockName;
	pc.bo = bo;
	pc.bindingPoint = bindingPoint;
	pendingConnections.push_back(pc);
}

void Program::replaceString(const std::string & replaceThis, const std::string & byThis) {
	replaceString(replaceThis, std::vector<std::string> {byThis});
}

void Program::replaceString(const std::string &replaceThis,
	const Strings &byThis) {
	if (!shaders.empty()) {
		ERRT("No se puede reemplazar la variable " + replaceThis +
			" porque ya hay shaders cargados");
	}
	subStrings[replaceThis] = byThis;
	release();
}

void Program::connectUniformBlock(std::shared_ptr<UniformBufferObject> bo,
	GLuint bindingPoint) {
	if (!bo)
		ERRT("Intentando instalar un Uniform Buffer Object vacío");
	replaceString("$" + bo->getBlockName(), bo->getDefinition());
	connectUniformBlock(bo->getBlockName(), bo, bindingPoint);
}

bool Program::bindUBOs() {

	for (uint i = 0; i < pendingConnections.size(); i++) {
		PendingConnection &pc = pendingConnections[i];
		if (!bindBlockToBindingPoint(pc.blockName, pc.bindingPoint)) {
			std::ostringstream os;
			os << "No se puede vincular el UBO \"" << pc.blockName;
			os << "\" asegúrate de que se usa en alguno de los fuentes del shader:"
				<< std::endl;
			printSrcs(os);
			ERRT(os.str());
		}
		PGUPV::gl_uniform_buffer.bindBufferBase(pc.bo, pc.bindingPoint);
	}

	return true;
}

uint Program::getUniformBlockSize(const std::string &blockName) {

	if (programId == 0)
		return 0;

	GLuint idx = glGetUniformBlockIndex(programId, blockName.c_str());
	if (idx == GL_INVALID_INDEX)
		return 0;
	GLint size;
	glGetActiveUniformBlockiv(programId, idx, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	return size;
}

int Program::getUniformBlockMemberOffset(const std::string &blockName,
	const std::string &member) {
	std::string name = blockName + "." + member;
	return getUniformBlockMemberOffset(name);
}

int Program::getUniformBlockMemberOffset(const std::string &member) {
	if (programId == 0)
		return -1;

	CHECK_GL();
	const char *pname = member.c_str();
	GLuint idx;

	glGetUniformIndices(programId, 1, &pname, &idx);
	if (idx == GL_INVALID_INDEX)
		return -1;

	GLint offset;
	glGetActiveUniformsiv(programId, 1, &idx, GL_UNIFORM_OFFSET, &offset);
	return offset;
}

int Program::getActiveUniformName(uint idx, std::string &name) const {
	if (programId == 0)
		return -1;

	CHECK_GL();

	char buffer[200];

	glGetActiveUniformName(programId, idx, sizeof(buffer), NULL, buffer);
	name = buffer;
	return 0;
}

int Program::getProgramInfo(GLenum pname) const {
	if (programId == 0)
		ERRT("El programa no está compilado");

	CHECK_GL();
	GLint data;

	glGetProgramiv(programId, pname, &data);
	return data;
}


std::string UniformInfo::toString() const {
	std::ostringstream os;
	GLSLTypeInfo t = PGUPV::getGLSLTypeInfo(type);
	os << name;
	if (offset >= 0)
		os << ", offset=" << offset;
	os << ", size=" << size << ", type=" << t.name;
	return os.str();
}

std::string UniformInfo::toString(void *bo) const {
	std::ostringstream os;
	GLSLTypeInfo t = PGUPV::getGLSLTypeInfo(type);
	os << name;
	if (offset >= 0)
		os << ", offset=" << offset;
	os << ", size=" << size << ", type=" << t.name;
	os << ", val="
		<< formatGLSLValue(t, size, reinterpret_cast<char *>(bo) + offset, true);
	return os.str();
}

void Program::printBoundShaderStorageBuffers(std::ostream &os) {
	os << "\n";
	os << "BOUND SHADER STORAGE BUFFERS\n";
	os << "---------------------\n";

	GLint ssbBindings;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &ssbBindings);
	for (int i = 0; i < ssbBindings; i++) {
		GLint bound;
		GLint64 size;
		glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, i, &bound);
		if (bound) {
			glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_SIZE, i, &size);
			os << "  " << i << ": Object: " << bound << "  Size: " << size
				<< std::endl;
		}
	}
}

#define CHECK_BINDING(b)                                                       \
  {                                                                            \
    GLint t = -1;                                                              \
    glGetIntegerv(b, &t);                                                      \
    if (t != 0)                                                                \
      temp << "  " #b ": " << std::to_string(t) << std::endl;                  \
  }

void Program::printBoundImages(std::ostream &os) {
	GLint numImageUnits;

	os << "\n";
	os << "BOUND IMAGE OBJECTS\n";
	os << "---------------------\n";

	glGetIntegerv(GL_MAX_IMAGE_UNITS, &numImageUnits);

	for (int i = 0; i < numImageUnits; i++) {
		GLint texture;
		CHECK_GL();
		glGetIntegeri_v(GL_IMAGE_BINDING_NAME, i, &texture);
		CHECK_GL();
		/*
		He comprobado que en algunas tarjetas AMD (Radeon HD 7400G), la llamada
		anterior genera el error:
		GL Debug: (ERROR) glGetIntegerIndexedv parameter <index> has an invalid
		value '0': must be within '0--1' when <target> is
		'GL_IMAGE_BINDING_ACCESS_EXT' (GL_INVALID_VALUE)
		Si te encuentras ese error, comenta este bucle entero.
		*/

		if (texture) {
			os << "IMAGE UNIT " << i << std::endl;
			os << "  Tex id: " << texture;
			GLint level, access;
			glGetIntegeri_v(GL_IMAGE_BINDING_LEVEL, i, &level);
			os << "  Level: " << level << "  ";
			glGetIntegeri_v(GL_IMAGE_BINDING_ACCESS, i, &access);
			if (access == GL_READ_ONLY)
				os << "GL_READ_ONLY\n";
			else if (access == GL_READ_WRITE)
				os << "GL_READ_WRITE\n";
			else if (access == GL_WRITE_ONLY)
				os << "GL_WRITE_ONLY\n";
			else
				ERRT("Unknown access mode");
		}
	}
}

void Program::printBoundTextures(std::ostream &os) {
	GLint numTextureUnits;
	GLint prevActiveTexture;

	os << "\n";
	os << "BOUND TEXTURE OBJECTS\n";
	os << "---------------------\n";

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numTextureUnits);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveTexture);

	for (int i = 0; i < numTextureUnits; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::ostringstream temp;

		CHECK_BINDING(GL_TEXTURE_BINDING_1D);
		CHECK_BINDING(GL_TEXTURE_BINDING_1D_ARRAY);
		CHECK_BINDING(GL_TEXTURE_BINDING_2D);
		CHECK_BINDING(GL_TEXTURE_BINDING_2D_ARRAY);
		CHECK_BINDING(GL_TEXTURE_BINDING_2D_MULTISAMPLE);
		CHECK_BINDING(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY);
		CHECK_BINDING(GL_TEXTURE_BINDING_3D);
		CHECK_BINDING(GL_TEXTURE_BINDING_BUFFER);
		CHECK_BINDING(GL_TEXTURE_BINDING_CUBE_MAP);
		CHECK_BINDING(GL_TEXTURE_BINDING_RECTANGLE);

		std::string bindings = temp.str();
		if (!bindings.empty()) {
			os << "GL_TEXTURE" << i << std::endl;
			os << bindings;
		}
	}
	glActiveTexture(prevActiveTexture);
}
#undef CHECK_BINDING

#define PRINT_ONE_INT(c)                                                       \
{                                                                         \
GLint one = -1;                                                                \
glGetIntegerv(c, &one);                                                        \
os << #c ": " << one << std::endl;                                             \
}

#define PRINT_ONE_INDEXED_INT(c, i)                                            \
{                                                                         \
GLint one = -1;                                                                \
glGetIntegeri_v(c, i, &one);                                                   \
os << #c "[" << i << "]: " << one << std::endl;                                \
}

#define PRINT_ONE_FLOAT(c)                                                     \
{                                                                         \
GLfloat one;                                                                   \
glGetFloatv(c, &one);                                                          \
os << #c ": " << one << std::endl;                                             \
}

#define PRINT_TWO_FLOATS(c)                                                    \
{                                                                         \
GLfloat two[2];                                                                \
glGetFloatv(c, two);                                                           \
os << #c ": " << two[0] << ", " << two[1] << std::endl;                        \
}

void Program::printOpenGLInfo(std::ostream &os, bool verbose) {

	os << std::endl;
	os << std::string(70, '=') << std::endl;
	os << "OPENGL INFO" << std::endl;
	os << std::string(70, '=') << std::endl;

	const char *v;

	os << "GL_VENDOR: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_VENDOR))) != NULL
			? v
			: "Could not obtain GL_VENDOR") << std::endl;
	os << "GL_RENDERER: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_RENDERER))) != NULL
			? v
			: "Could not obtain GL_RENDERER") << std::endl;
	os << "GL_VERSION: "
		<< ((v = reinterpret_cast<const char *>(glGetString(GL_VERSION))) != NULL
			? v
			: "Could not obtain GL_VERSION") << std::endl;
	os << "GL_SHADING_LANGUAGE_VERSION: "
		<< ((v = reinterpret_cast<const char *>(
			glGetString(GL_SHADING_LANGUAGE_VERSION))) != NULL
			? v
			: "Could not obtain GL_SHADING_LANGUAGE_VERSION") << std::endl;

	if (verbose) {
		os << std::endl << "Extensiones OPENGL (" << getNumAvailableExtensions() << " disponibles)"
			<< std::endl << "------------------" << std::endl;
		printAvailableGLExtensions(os);
		os << std::endl << std::endl;
	}
	os << "OpenGL 2 y anteriores" << std::endl;
	PRINT_ONE_INT(GL_MAX_DRAW_BUFFERS);
	PRINT_ONE_INT(GL_MAX_VERTEX_ATTRIBS);
	PRINT_TWO_FLOATS(GL_POINT_SIZE_RANGE);
	PRINT_ONE_FLOAT(GL_POINT_SIZE_GRANULARITY);
	PRINT_ONE_INT(GL_MAX_TEXTURE_SIZE);
	PRINT_ONE_INT(GL_MAX_3D_TEXTURE_SIZE);
	PRINT_ONE_INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	PRINT_ONE_INT(GL_MAX_TEXTURE_IMAGE_UNITS);
	PRINT_ONE_INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

	os << "OpenGL 3" << std::endl;
	PRINT_ONE_INT(GL_MAX_RENDERBUFFER_SIZE);
	PRINT_ONE_INT(GL_MAX_COLOR_ATTACHMENTS);
	PRINT_ONE_INT(GL_MAX_VERTEX_UNIFORM_BLOCKS);
	PRINT_ONE_INT(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
	PRINT_ONE_INT(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
	PRINT_ONE_INT(GL_MAX_COMBINED_UNIFORM_BLOCKS);
	PRINT_ONE_INT(GL_MAX_UNIFORM_BUFFER_BINDINGS);
	PRINT_ONE_INT(GL_MAX_UNIFORM_BLOCK_SIZE);
	PRINT_ONE_INT(GL_MAX_GEOMETRY_OUTPUT_VERTICES);
	PRINT_ONE_INT(GL_MAX_ARRAY_TEXTURE_LAYERS);
	PRINT_ONE_INT(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
	PRINT_ONE_INT(GL_MAX_TEXTURE_BUFFER_SIZE);

	// Lo vuelvo a pedir aquí para no crear una dependencia con Window
	GLint maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);

	if (maj == 4) {
		os << "OpenGL 4.0" << std::endl;
		PRINT_ONE_INT(GL_MAX_SUBROUTINES);
		PRINT_ONE_INT(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS);
		PRINT_ONE_INT(GL_MAX_PATCH_VERTICES);
		PRINT_ONE_INT(GL_MAX_TESS_GEN_LEVEL);

		PRINT_ONE_INT(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS);
		PRINT_ONE_INT(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS);
		PRINT_ONE_INT(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS);

		PRINT_ONE_INT(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS);
		PRINT_ONE_INT(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS);

		if (min >= 1) {
			os << "OpenGL 4.1" << std::endl;
			PRINT_ONE_INT(GL_MAX_VIEWPORTS);
		}
		if (min >= 2) {
			os << "OpenGL 4.2" << std::endl;
			PRINT_ONE_INT(GL_MAX_IMAGE_UNITS);
		}
		if (min >= 3) {
			os << "OpenGL 4.3" << std::endl;
			PRINT_ONE_INT(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1);
			PRINT_ONE_INDEXED_INT(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2);
			PRINT_ONE_INT(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE);

			PRINT_ONE_INT(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
			PRINT_ONE_INT(GL_MAX_SHADER_STORAGE_BLOCK_SIZE);
			PRINT_ONE_INT(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS);
			PRINT_ONE_INT(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS);
		}
	}

	printBoundTextures(os);
	if (maj >= 4 && min >= 3) {
		printBoundImages(os);
		printBoundShaderStorageBuffers(os);
	}
}

#undef PRINT_ONE_INT
#undef PRINT_ONE_FLOAT
#undef PRINT_TWO_FLOATS

// Aquí adaptamos la entrada a lo que espera formatGLSLValue
std::string printUniformValue(GLint programId, GLint loc, GLenum type,
	uint size) {
	PGUPV::GLSLTypeInfo t = PGUPV::getGLSLTypeInfo(type);
	std::ostringstream result;
	char vals[sizeof(glm::dmat4)]; // el tipo más grande

	if (t.baseTypeGLEnum == GL_FLOAT) {
		glGetUniformfv(programId, loc, reinterpret_cast<GLfloat *>(vals));
	}
	else if (t.baseTypeGLEnum == GL_INT || t.baseTypeGLEnum == 0) {
		glGetUniformiv(programId, loc, reinterpret_cast<GLint *>(vals));
		return PGUPV::formatGLSLValue(t, size, &vals[0]);
	}
	else if (t.baseTypeGLEnum == GL_UNSIGNED_INT) {
		glGetUniformuiv(programId, loc, reinterpret_cast<GLuint *>(vals));
	}
	else if (t.baseTypeGLEnum == GL_BOOL) {
		glGetUniformiv(programId, loc, reinterpret_cast<GLint *>(vals));
	}
	else
		return "[Tipo desconocido]";
	return PGUPV::formatGLSLValue(t, size, &vals[0]);
}

// Imprime en el flujo indicado un montón de información sobre el shader (una
// vez enlazado)
void Program::printInfo(std::ostream &os) const {

	os << std::endl;
	os << std::string(70, '=') << std::endl;
	os << "Id de programa: " << programId;
	os << "  GL_DELETE_STATUS: " << (getProgramInfo(GL_DELETE_STATUS) ? "GL_TRUE" : "GL_FALSE");
	os << "  GL_LINK_STATUS: " << (getProgramInfo(GL_LINK_STATUS) ? "GL_TRUE" : "GL_FALSE");
	os << "  GL_VALIDATE_STATUS: " << (getProgramInfo(GL_VALIDATE_STATUS) ? "GL_TRUE" : "GL_FALSE") << std::endl;
	printSrcs(os);
	os << std::string(70, '-') << std::endl;

	if (shaders.empty()) {
		os << "Este programa no contiene shaders\n";
		return;
	}

	os << "GL_ATTACHED_SHADERS: " << getProgramInfo(GL_ATTACHED_SHADERS)
		<< std::endl;
	int aat = getProgramInfo(GL_ACTIVE_ATTRIBUTES);
	os << "GL_ACTIVE_ATTRIBUTES: " << aat << std::endl;

	for (int i = 0; i < aat; i++) {
		char buffer[200];
		GLint size;
		GLenum type;

		glGetActiveAttrib(programId, i, sizeof(buffer), NULL, &size, &type, buffer);

		os << "  [" << i << "]: " << buffer << ", size= " << size;
		const GLSLTypeInfo &t = getGLSLTypeInfo(type);
		os << ", type= " << t.name << std::endl;
	}

	UniformInfoBlocks uniforms = getActiveUniforms();

	os << "GL_ACTIVE_UNIFORMS: " << uniforms.totalActiveUniforms << std::endl;

	if (uniforms.defaultBlockUniforms.size() > 0) {
		os << "  Default Uniform Block members:" << std::endl;
		for (const auto i : uniforms.defaultBlockUniforms) {
			os << "       " << i.toString();
			GLint loc;
			loc = glGetUniformLocation(programId, i.name.c_str());
			if (loc == -1)
				os << ": localización desconocida (es una variable predefinida?)";
			else {
				os << ", loc: " << loc
					<< ", val: " << printUniformValue(programId, loc, i.type, i.size);
			}
			os << std::endl;
		}
	}

	os << "GL_ACTIVE_UNIFORM_BLOCKS: " << uniforms.blocks.size() << std::endl;

	for (uint i = 0; i < uniforms.blocks.size(); i++) {
		os << "  [" << i << "]: " << uniforms.blocks[i].name << "\n";
		os << "     GL_UNIFORM_BLOCK_BINDING: " << uniforms.blocks[i].binding
			<< "\n";
		os << "     GL_UNIFORM_BLOCK_DATA_SIZE: " << uniforms.blocks[i].blockSize
			<< "\n";
		os << "     Members:\n";
		GLint boId;
		glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, uniforms.blocks[i].binding,
			&boId);
		if (boId == 0) {
			os << " No hay un buffer object vinculado. Esto no debería pasar\n";
			continue;
		}
		glBindBuffer(GL_COPY_READ_BUFFER, boId);
		void *boContent = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
		if (boContent == nullptr) {
			os << " [Buffer object vacío]\n";
			continue;
		}
		std::sort(uniforms.blocks[i].uniforms.begin(),
			uniforms.blocks[i].uniforms.end(),
			[](const UniformInfo &a, const UniformInfo &b) {
			return a.offset < b.offset;
		});
		for (const auto j : uniforms.blocks[i].uniforms) {
			os << "       " << j.toString(boContent) << "\n";
		}
		glUnmapBuffer(GL_COPY_READ_BUFFER);
	}

	// Lo vuelvo a pedir aquí para no crear una dependencia con Window
	GLint maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);

	int glVersion = maj * 100 + min;

	/// Shader Storage Blocks sólo disponibles a partir de GL 4.3
	if (glVersion >= 403) {
		GLint activeResources;
		os << "GL_SHADER_STORAGE_BLOCK";
		glGetProgramInterfaceiv(getId(), GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &activeResources);
		os << ": " << activeResources << "\n";

		char buffer[200];
		for (int activeResource = 0; activeResource < activeResources; activeResource++) {
			glGetProgramResourceName(getId(), GL_SHADER_STORAGE_BLOCK, activeResource, sizeof(buffer), nullptr, buffer);
			os << "  [" << activeResource << "]: " << buffer << "\n";

			// Cuidado! No cambiar el orden de los elementos: se usa más abajo
			GLenum props[] = { GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE,
			  GL_REFERENCED_BY_VERTEX_SHADER, GL_REFERENCED_BY_TESS_CONTROL_SHADER, GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
			  GL_REFERENCED_BY_GEOMETRY_SHADER, GL_REFERENCED_BY_FRAGMENT_SHADER, GL_REFERENCED_BY_COMPUTE_SHADER };
			const int numProps = sizeof(props) / sizeof(props[0]);
			GLint iparams[numProps];
			GLint retCount;

			glGetProgramResourceiv(getId(), GL_SHADER_STORAGE_BLOCK, activeResource, numProps, props, numProps, &retCount, iparams);
			assert(retCount == numProps);

			os << "     GL_BUFFER_BINDING: " << iparams[0] << std::endl;
			os << "     GL_BUFFER_DATA_SIZE: " << iparams[1] << std::endl;
			os << "     Used by: ";
			if (iparams[2]) os << "vertex ";
			if (iparams[3]) os << "tess_control ";
			if (iparams[4]) os << "tess_eval ";
			if (iparams[5]) os << "geometry ";
			if (iparams[6]) os << "fragment ";
			if (iparams[7]) os << "compute ";
			os << "\n";

			GLint numActiveVariables;
			GLenum numActiveVariablesEnum = GL_NUM_ACTIVE_VARIABLES;
			glGetProgramResourceiv(getId(), GL_SHADER_STORAGE_BLOCK, activeResource, 1, &numActiveVariablesEnum, 1, &retCount, &numActiveVariables);
			if (retCount != 1 || numActiveVariables <= 0)
				os << "     No active variables\n";
			else {
				os << "     Members:\n";
				auto activeVariablesIdx = new GLint[numActiveVariables];
				GLenum activeVariablesEnum = GL_ACTIVE_VARIABLES;
				glGetProgramResourceiv(getId(), GL_SHADER_STORAGE_BLOCK, activeResource, 1, &activeVariablesEnum, numActiveVariables, &retCount, activeVariablesIdx);
				assert(numActiveVariables == retCount);

				// No cambiar el orden de los elementos. Se usa más abajo
				GLenum varProps[] = {
				  GL_TYPE, GL_ARRAY_SIZE, GL_ARRAY_STRIDE, GL_IS_ROW_MAJOR, // 0..3
				  GL_MATRIX_STRIDE, GL_OFFSET, GL_TOP_LEVEL_ARRAY_SIZE, GL_TOP_LEVEL_ARRAY_STRIDE }; // 4..7
				GLint numVarProps = sizeof(varProps) / sizeof(varProps[0]);

				const int varPropResSize = 20;
				GLint varVals[varPropResSize];
				for (int i = 0; i < numActiveVariables; i++) {
					char name[200];
					glGetProgramResourceName(getId(), GL_BUFFER_VARIABLE, activeVariablesIdx[i], sizeof(name), nullptr, name);
					os << "     " << name;
					glGetProgramResourceiv(getId(), GL_BUFFER_VARIABLE, activeVariablesIdx[i], numVarProps, varProps, varPropResSize, &retCount, varVals);
					auto t = getGLSLTypeInfo(varVals[0]);

					os << ", offset=" << varVals[5] << ", type=" << t.name << ", array_size=" << varVals[1] << ", array_stride=" << varVals[2];
					os << ", row_major=" << varVals[3] << ", matrix_stride=" << varVals[4] << ", top_level_array_size=" << varVals[6];
					os << ", top_level_array_stride=" << varVals[7] << "\n";
				}
				delete[] activeVariablesIdx;
			}
		}
	}
#ifndef __APPLE__
	if (shaders.find(Shader::COMPUTE_SHADER) != shaders.end()) {
		int size[3];
		glGetProgramiv(programId, GL_COMPUTE_WORK_GROUP_SIZE, size);
		os << "GL_COMPUTE_WORK_GROUP_SIZE: " << size[0] << " x " << size[1] << " x "
			<< size[2] << "\n";
	}
#endif
}

void findOtherArrayMembers(const GLint programId, const std::string &name,
	uint size, GLint type,
	std::vector<UniformInfo> &dest) {
	size_t posB = name.find_first_of("[");
	std::string base(name, 0, posB);

	for (uint i = 1; i < size; i++) {
		std::ostringstream uniformName;
		uniformName << base << "[" << std::to_string(i) << "]";
		GLint loc = glGetUniformLocation(programId, uniformName.str().c_str());
		if (loc < 0) {
			ERR(base + " es de tipo array con " + std::to_string(size) +
				" elementos, pero no encuentro " + uniformName.str());
		}
		dest.push_back(UniformInfo(uniformName.str(), 0, 1, type));
	}
}


UniformInfoBlocks Program::getActiveUniforms() const {

	uint nuniblocks = getProgramInfo(GL_ACTIVE_UNIFORM_BLOCKS);
	UniformInfoBlocks ublocks;

	ublocks.blocks.resize(nuniblocks);

	ublocks.totalActiveUniforms = getProgramInfo(GL_ACTIVE_UNIFORMS);
	for (uint i = 0; i < ublocks.totalActiveUniforms; i++) {
		std::string un;
		getActiveUniformName(i, un);
		GLint offset, size, type, blockIdx;
		glGetActiveUniformsiv(programId, 1, &i, GL_UNIFORM_OFFSET, &offset);
		glGetActiveUniformsiv(programId, 1, &i, GL_UNIFORM_SIZE, &size);
		glGetActiveUniformsiv(programId, 1, &i, GL_UNIFORM_TYPE, &type);
		glGetActiveUniformsiv(programId, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIdx);
		if (blockIdx == -1) {
			UniformInfo uniform(un, offset, size, type);
			if (size > 1) {
				uniform.size = 1;
				ublocks.defaultBlockUniforms.push_back(uniform);
				findOtherArrayMembers(programId, un, size, type,
					ublocks.defaultBlockUniforms);
			}
			else {
				ublocks.defaultBlockUniforms.push_back(uniform);
			}
		}
		else {
			ublocks.blocks[blockIdx].uniforms.push_back(
				UniformInfo(un, offset, size, type));
		}
	}

	for (uint i = 0; i < nuniblocks; i++) {
		char bname[200];
		glGetActiveUniformBlockName(programId, i, sizeof(bname), NULL, bname);
		ublocks.blocks[i].name = bname;
		glGetActiveUniformBlockiv(programId, i, GL_UNIFORM_BLOCK_BINDING,
			&ublocks.blocks[i].binding);
		glGetActiveUniformBlockiv(programId, i, GL_UNIFORM_BLOCK_DATA_SIZE,
			&ublocks.blocks[i].blockSize);
	}

	return ublocks;
}

bool Program::hasShaderType(Shader::ShaderType type) const {
	return getShader(type).get() != NULL;
}

void Program::printSrcs(std::ostream &os) const {
	for (std::map<Shader::ShaderType, std::shared_ptr<Shader>>::const_iterator i =
		shaders.cbegin();
		i != shaders.cend(); ++i) {
		(i->second)->printSrc(os, true, true);
	}
}

/**
Añade el shader proporcionado al programa. El shader debe estar listo para su
compilación (no se expandirán cadenas ni se preprocesará el código fuente).
Si ya existiera un shader con el mismo tipo, lo reemplazaría.
\param shader Shader a incluir en el programa
*/
void Program::addShader(std::shared_ptr<Shader> shader) {
	auto i = shaders.find(shader->getType());
	if (i != shaders.end())
		shaders.erase(i);
	shaders.insert(std::pair<Shader::ShaderType, std::shared_ptr<Shader>>(
		shader->getType(), shader));
#ifndef __APPLE__
	if (shaders.find(Shader::COMPUTE_SHADER) != shaders.end() &&
		shaders.size() > 1)
		ERRT("Un shader de computación no puede enlazarse con ningún otro tipo "
			"de shader");
#endif
	release();
}

/**
Desvincula el shader del tipo indicado del programa. El shader puede seguir
usándose en otros
programas
*/
void Program::removeShader(Shader::ShaderType type) {
	shaders.erase(type);
	release();
}

// Devuelve la localización del uniform de tipo subrutina indicado en el
// shader
// de tipo indicado
uint Program::getSubroutineUniformLocation(Shader::ShaderType type,
	std::string name) {
	if (programId == 0)
		ERRT("No se puede pedir la posición de un uniform si el shader no está "
			"enlazado");

	GLint loc = glGetSubroutineUniformLocation(programId, Shader::toGLType(type),
		name.c_str());
	if (loc < 0) {
		ERRT("No se encuentra el uniform de subrutina " + name + " en el " +
			Shader::toFriendlyName(type));
	}
	CHECK_GL();
	return loc;
}

// Devuelve el índice de la subrutina indicada
GLuint Program::getSubroutineIndex(Shader::ShaderType type, std::string name) {
	if (programId == 0)
		ERRT("No se puede pedir la posición de un uniform si el shader no está "
			"enlazado");

	GLuint loc =
		glGetSubroutineIndex(programId, Shader::toGLType(type), name.c_str());
	if (loc == GL_INVALID_INDEX) {
		ERRT("La subrutina " + name + " no se encuentra en el " +
			Shader::toFriendlyName(type));
	}
	CHECK_GL();
	return loc;
}

void Program::setRoutine(Shader::ShaderType type, std::string uniformRoutine,
	std::string routineName) {
	uint uniformId = getSubroutineUniformLocation(type, uniformRoutine);
	GLuint routineId = getSubroutineIndex(type, routineName);
	if (subrutinas[type].size() <= uniformId)
		subrutinas[type].resize(uniformId + 1, GL_INVALID_INDEX);
	subrutinas[type][uniformId] = routineId;
}

void Program::refreshRoutineUniforms() {
	CHECK_GL();
	for (int i = 0; i < Shader::NUM_SHADER_TYPES; i++) {
		Shader::ShaderType type = (Shader::ShaderType)i;
#ifdef _DEBUG
		// Durante el desarrollo, comprobaremos que los índices son válidos y
		// que hay todos los que son. En release, no se comprobará.
		GLint n;
		glGetProgramStageiv(programId, Shader::toGLType(type),
			GL_ACTIVE_SUBROUTINE_UNIFORMS, &n);
		if (static_cast<size_t>(n) != subrutinas[i].size()) {
			ERRT("No has establecido la subrutina de todos los uniforms de tipo "
				"subrutina de tu " +
				Shader::toFriendlyName(type));
		}
#endif

		if (subrutinas[i].size() > 0) {
#ifdef _DEBUG
			// Durante el desarrollo, comprobaremos que los índices son válidos y
			// que hay todos los que son
			for (uint j = 0; j < subrutinas[i].size(); j++) {
				if (subrutinas[i][j] == GL_INVALID_INDEX)
					ERRT("No has establecido la subrutina de todos los uniforms de "
						"tipo subrutina de tu " +
						Shader::toFriendlyName(type));
			}
#endif
			glUniformSubroutinesuiv(Shader::toGLType(type), gsl::narrow<GLsizei>(subrutinas[i].size()),
				&subrutinas[i][0]);
			CHECK_GL2("Posiblemente no has establecido la subrutina de todos los "
				"uniforms de tipo subrutina de tu " +
				Shader::toFriendlyName(type));
		}
	}
}

void Program::setTransformFeedbackVaryings(std::vector<std::string> vars, bool interleaved) {
	transformVaryings = vars;
	transformInterleaved = interleaved;
	release();
}

long long Program::getModificationTime() {
	long long newest = 0;
	for (auto s : shaders) {
		newest = MAX(s.second->getModificationTime(), newest);
	}
	return newest;
}
