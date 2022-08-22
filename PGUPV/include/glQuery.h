#pragma once

// 2018

#include <GL/glew.h>
#include <cstdint>

namespace PGUPV {
	class GLQuery {
	public:
		GLQuery(GLenum type) : queryType(type), queryId(0) {
			
		}
		void begin() {
			if (!queryId) glGenQueries(1, &queryId);
			glBeginQuery(queryType, queryId);
		}
		void end() {
			glEndQuery(queryType);
		}
		uint32_t getValueU32() {
			uint32_t result;
			if (!queryId) return 0;
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &result);
			return result;
		}
		uint64_t getValueU64() {
			uint64_t result;
			if (!queryId) return 0;
			glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &result);
			return result;
		}
	private:
		GLenum queryType;
		GLuint queryId;
	};
};