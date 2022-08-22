
#include <string>
#include <cstring>
#include "drawCommand.h"
#include "log.h"

using PGUPV::DrawCommand;
using PGUPV::DrawArrays;
using PGUPV::DrawElements;
using PGUPV::MultiDrawArrays;
using PGUPV::MultiDrawElements;
using PGUPV::MultiDrawElementsBaseVertex;
using PGUPV::TriangleIndices;


void DrawCommand::render() {
  if (mode == GL_PATCHES) {
    glPatchParameteri(GL_PATCH_VERTICES, verticesPerPatch);
  }
  if (restartPrimitive) {
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(restartIndex);
  }

  renderFunc();

  if (restartPrimitive) {
    glDisable(GL_PRIMITIVE_RESTART);
  }
}


// Activa/desactiva el reinicio de primitivas
void DrawCommand::setPrimitiveRestart(bool restart) {
  restartPrimitive = restart;
}

// Establece el valor del índice de reinicio
void DrawCommand::setRestartIndex(uint rindex) {
  restartIndex = rindex;
}

std::vector<TriangleIndices> DrawCommand::getTrianglesIndices(void * /*indicesBuffer*/)
{
  ERRT("No implementado. Habla con Paco");
}

std::vector<TriangleIndices> DrawArrays::getTrianglesIndices(void * /*indicesBuffer*/) {
  std::vector<TriangleIndices> result;

  assert(!restartPrimitive);

  switch (mode) {
  case GL_TRIANGLES:
    for (int i = 0; i + 3 <= count; i += 3) {
      TriangleIndices t(first + i, first + i + 1, first + i + 2);
      result.push_back(t);
    }
    break;
  case GL_TRIANGLE_STRIP:
    for (int i = 0; i < count - 2; i++) {
      TriangleIndices t(first + i + 2, first + i, first + i + 1);
      if ((i & 1) == 1) {
        std::swap(t.idx[1], t.idx[2]);
      }
      result.push_back(t);
    }
    break;
  case GL_TRIANGLE_FAN:
    for (int i = 0; i < count - 2; i++) {
      TriangleIndices t(first + i + 2, first, first + i + 1);
      result.push_back(t);
    }
    break;
  default:
    ERRT("La primitiva no es de tipo triángulo");
  }
  return result;
}

static GLuint getIndex(void *buffer, GLenum type, uint which) {
  if (type == GL_UNSIGNED_SHORT)
    return *(static_cast<const GLushort *>(buffer)+which);
  if (type == GL_UNSIGNED_BYTE)
    return *(static_cast<const GLubyte *>(buffer)+which);
  else
    return *(static_cast<const GLuint *>(buffer)+which);
}

std::vector<TriangleIndices> DrawElements::getTrianglesIndices(void *indicesBuffer) {
  std::vector<TriangleIndices> result;

  void *idxBuffer = static_cast<char *>(indicesBuffer)+(static_cast<const char *>(offset)-static_cast<char *>(0));

  switch (mode) {
  case GL_TRIANGLES:
    assert(!restartPrimitive);
    for (int i = 0; i + 3 <= count; i += 3) {
      TriangleIndices t(
        getIndex(idxBuffer, type, i),
        getIndex(idxBuffer, type, i + 1),
        getIndex(idxBuffer, type, i + 2));
      result.push_back(t);
    }
    break;
  case GL_TRIANGLE_STRIP:
  {
    bool even = true;
    for (int i = 0; i < count - 2; i++) {
      TriangleIndices t(
        getIndex(idxBuffer, type, i + 2),
        getIndex(idxBuffer, type, i),
        getIndex(idxBuffer, type, i + 1));
      if (restartPrimitive && t.idx[0] == restartIndex) {
        even = true;
        i += 2;
        continue;
      }
      if (!even) {
        std::swap(t.idx[1], t.idx[2]);
      }
      even = !even;
      result.push_back(t);
    }
  }
  break;
  case GL_TRIANGLE_FAN:
  {
    uint currentFirst = 0;
    for (int i = 0; i < count - 2; i++) {
      TriangleIndices t(
        getIndex(idxBuffer, type, i + 2),
        getIndex(idxBuffer, type, currentFirst),
        getIndex(idxBuffer, type, i + 1));
      if (restartPrimitive && t.idx[0] == restartIndex) {
        currentFirst = i + 3;
        i += 2;
        continue;
      }
      result.push_back(t);
    }
  }
  break;
  default:
    ERRT("La primitiva no es de tipo triángulo");
  }
  return result;
}

MultiDrawArrays::MultiDrawArrays(GLenum mode, const GLint *first, const GLint *count, GLsizei primcount) :
DrawCommand(mode), primcount(primcount), first(primcount), count(primcount)  {
  memcpy(&this->first[0], first, sizeof(GLint) * primcount);
  memcpy(&this->count[0], count, sizeof(GLint) * primcount);
}


MultiDrawElements::MultiDrawElements(GLenum mode, const GLint *count, GLenum type, const void * const *indices, GLsizei primcount) :
DrawCommand(mode), count(primcount), type(type), indices(primcount), primcount(primcount) {
  memcpy(&this->count[0], count, sizeof(GLint) * primcount);
  memcpy(&this->indices[0], indices, sizeof(void *) * primcount);
}

MultiDrawElementsBaseVertex::MultiDrawElementsBaseVertex(GLenum mode, const GLint *count, GLenum type, void **indices,
  GLsizei primcount, const GLint *baseVertex) :
  DrawCommand(mode), count(primcount), type(type), indices(primcount), primcount(primcount), baseVertex(primcount) {
  memcpy(&this->count[0], count, sizeof(GLint) * primcount);
  memcpy(&this->indices[0], indices, sizeof(void *) * primcount);
  memcpy(&this->baseVertex[0], baseVertex, sizeof(GLint) * primcount);
}
