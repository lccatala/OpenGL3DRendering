#ifndef _UNIFORM_H
#define _UNIFORM_H 2014

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace PGUPV {
class UniformWriter {
public:
	inline static void write(GLint loc, const GLfloat v) {
		glUniform1f(loc, v);
	}

	inline static void write(GLint loc, const glm::vec2 &v) {
		glUniform2f(loc, v.x, v.y);
	}

	inline static void write(GLint loc, const glm::vec3 &v) {
		glUniform3f(loc, v.x, v.y, v.z);
	}

	inline static void write(GLint loc, const glm::vec4 &v) {
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}

	inline static void write(GLint loc, const GLdouble v) {
		glUniform1d(loc, v);
	}

	inline static void write(GLint loc, const glm::dvec2 &v) {
		glUniform2d(loc, v.x, v.y);
	}

	inline static void write(GLint loc, const glm::dvec3 &v) {
		glUniform3d(loc, v.x, v.y, v.z);
	}

	inline static void write(GLint loc, const glm::dvec4 &v) {
		glUniform4d(loc, v.x, v.y, v.z, v.w);
	}

	inline static void write(GLint loc, const GLint v) {
		glUniform1i(loc, v);
	}

	inline static void write(GLint loc, const glm::ivec2 &v) {
		glUniform2i(loc, v.x, v.y);
	}

	inline static void write(GLint loc, const glm::ivec3 &v) {
		glUniform3i(loc, v.x, v.y, v.z);
	}

	inline static void write(GLint loc, const glm::ivec4 &v) {
		glUniform4i(loc, v.x, v.y, v.z, v.w);
	}

	inline static void write(GLint loc, const GLuint v) {
		glUniform1ui(loc, v);
	}

	inline static void write(GLint loc, const glm::uvec2 &v) {
		glUniform2ui(loc, v.x, v.y);
	}

	inline static void write(GLint loc, const glm::uvec3 &v) {
		glUniform3ui(loc, v.x, v.y, v.z);
	}

	inline static void write(GLint loc, const glm::uvec4 &v) {
		glUniform4ui(loc, v.x, v.y, v.z, v.w);
	}

};

}; // namespace


#endif // !_UNIFORM_H
