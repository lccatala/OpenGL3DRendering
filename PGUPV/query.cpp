
#include "query.h"

using PGUPV::Query;

Query::Query(GLenum type) : type(type), stream(-1)
{
    glGenQueries(1, &id);
}

Query::~Query() {
    glDeleteQueries(1, &id);
}


void Query::begin() {
    glBeginQuery(type, id);
}

void Query::beginIndexed(unsigned int strm) {
    this->stream = strm;
    glBeginQueryIndexed(type, strm, id);
}


void Query::end() {
    if (stream != -1)
        glEndQueryIndexed(type, stream);
    else
        glEndQuery(type);
}

bool Query::isResultAvailable() {
    GLuint available;
    glGetQueryObjectuiv(id, GL_QUERY_RESULT_AVAILABLE, &available);
    return available != 0;
}

GLuint Query::getResult() {
    GLuint res;
    glGetQueryObjectuiv(id, GL_QUERY_RESULT, &res);
    return res;
}

GLint64 Query::getResult64() {
    GLuint64 res;
    glGetQueryObjectui64v(id, GL_QUERY_RESULT, &res);
    return res;

}

GLint64 Query::getGLTimeStamp() {
    GLint64 t;
    glGetInteger64v(GL_TIMESTAMP, &t);
    return t;
}

