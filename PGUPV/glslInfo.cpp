
#include <sstream>

#include "log.h"
#include "glslInfo.h"

using PGUPV::GLSLTypeInfo;


uint roundUpToVec4(uint s) {
  return ((s - 1) / 16 + 1) * 16;
}

static const GLSLTypeInfo unknownGLSLType = { 0, "UNKNOWN TYPE", 0, 0, 0, 0, 0, 0 };

#define C(T, NCOMP, SIZE_BASE_TYPE, GL_BASE_TYPE, COLS, ROWS)                  \
{                                                                            \
    T, std::string(#T), NCOMP, GL_BASE_TYPE, SIZE_BASE_TYPE, COLS, ROWS,       \
    roundUpToVec4(COLS *ROWS *SIZE_BASE_TYPE)                                             \
}
#define NC(T)                                                                  \
{ T, std::string(#T), 0, 0, T, 0, 0, 0}

static const GLSLTypeInfo glslTypeInfo[] = {
  C(GL_FLOAT, 1, sizeof(GLfloat), GL_FLOAT, 1, 1),
  C(GL_FLOAT_VEC2, 2, sizeof(GLfloat), GL_FLOAT, 1, 2),
  C(GL_FLOAT_VEC3, 3, sizeof(GLfloat), GL_FLOAT, 1, 3),
  C(GL_FLOAT_VEC4, 4, sizeof(GLfloat), GL_FLOAT, 1, 4),
  C(GL_FLOAT_MAT2, 4, sizeof(GLfloat), GL_FLOAT, 2, 2),
  C(GL_FLOAT_MAT3, 9, sizeof(GLfloat), GL_FLOAT, 3, 3),
  C(GL_FLOAT_MAT4, 16, sizeof(GLfloat), GL_FLOAT, 4, 4),
  C(GL_FLOAT_MAT2x3, 6, sizeof(GLfloat), GL_FLOAT, 2, 3),
  C(GL_FLOAT_MAT2x4, 8, sizeof(GLfloat), GL_FLOAT, 2, 4),
  C(GL_FLOAT_MAT3x2, 6, sizeof(GLfloat), GL_FLOAT, 3, 2),
  C(GL_FLOAT_MAT3x4, 12, sizeof(GLfloat), GL_FLOAT, 3, 4),
  C(GL_FLOAT_MAT4x2, 8, sizeof(GLfloat), GL_FLOAT, 4, 2),
  C(GL_FLOAT_MAT4x3, 12, sizeof(GLfloat), GL_FLOAT, 4, 3),

  C(GL_DOUBLE, 1, sizeof(GLdouble), GL_DOUBLE, 1, 1),
  C(GL_DOUBLE_VEC2, 2, sizeof(GLdouble), GL_DOUBLE, 1, 2),
  C(GL_DOUBLE_VEC3, 3, sizeof(GLdouble), GL_DOUBLE, 1, 3),
  C(GL_DOUBLE_VEC4, 4, sizeof(GLdouble), GL_DOUBLE, 1, 4),
  C(GL_DOUBLE_MAT2, 4, sizeof(GLdouble), GL_DOUBLE, 2, 2),
  C(GL_DOUBLE_MAT3, 9, sizeof(GLdouble), GL_DOUBLE, 3, 3),
  C(GL_DOUBLE_MAT4, 16, sizeof(GLdouble), GL_DOUBLE, 4, 4),
  C(GL_DOUBLE_MAT2x3, 6, sizeof(GLdouble), GL_DOUBLE, 2, 3),
  C(GL_DOUBLE_MAT2x4, 8, sizeof(GLdouble), GL_DOUBLE, 2, 4),
  C(GL_DOUBLE_MAT3x2, 6, sizeof(GLdouble), GL_DOUBLE, 3, 2),
  C(GL_DOUBLE_MAT3x4, 12, sizeof(GLdouble), GL_DOUBLE, 3, 4),
  C(GL_DOUBLE_MAT4x2, 8, sizeof(GLdouble), GL_DOUBLE, 4, 2),
  C(GL_DOUBLE_MAT4x3, 12, sizeof(GLdouble), GL_DOUBLE, 4, 3),

  C(GL_INT, 1, sizeof(GLint), GL_INT, 1, 1),
  C(GL_INT_VEC2, 2, sizeof(GLint), GL_INT, 1, 2),
  C(GL_INT_VEC3, 3, sizeof(GLint), GL_INT, 1, 3),
  C(GL_INT_VEC4, 4, sizeof(GLint), GL_INT, 1, 4),

  C(GL_UNSIGNED_INT, 1, sizeof(GLuint), GL_UNSIGNED_INT, 1, 1),
  C(GL_UNSIGNED_INT_VEC2, 2, sizeof(GLuint), GL_UNSIGNED_INT, 1, 2),
  C(GL_UNSIGNED_INT_VEC3, 3, sizeof(GLuint), GL_UNSIGNED_INT, 1, 3),
  C(GL_UNSIGNED_INT_VEC4, 4, sizeof(GLuint), GL_UNSIGNED_INT, 1, 4),

  C(GL_BOOL, 1, sizeof(GLboolean), GL_BOOL, 1, 1),
  C(GL_BOOL_VEC2, 2, sizeof(GLboolean), GL_BOOL, 1, 2),
  C(GL_BOOL_VEC3, 3, sizeof(GLboolean), GL_BOOL, 1, 3),
  C(GL_BOOL_VEC4, 4, sizeof(GLboolean), GL_BOOL, 1, 4),

  NC(GL_SAMPLER_1D), NC(GL_SAMPLER_2D), NC(GL_SAMPLER_3D),
  NC(GL_SAMPLER_CUBE), NC(GL_SAMPLER_1D_SHADOW), NC(GL_SAMPLER_2D_SHADOW),
  NC(GL_SAMPLER_1D_ARRAY), NC(GL_SAMPLER_2D_ARRAY),
  NC(GL_SAMPLER_1D_ARRAY_SHADOW), NC(GL_SAMPLER_2D_ARRAY_SHADOW),
  NC(GL_SAMPLER_2D_MULTISAMPLE), NC(GL_SAMPLER_2D_MULTISAMPLE_ARRAY),
  NC(GL_SAMPLER_CUBE_SHADOW), NC(GL_SAMPLER_BUFFER), NC(GL_SAMPLER_2D_RECT),
  NC(GL_SAMPLER_2D_RECT_SHADOW), NC(GL_IMAGE_1D), NC(GL_IMAGE_2D),
  NC(GL_IMAGE_3D), NC(GL_IMAGE_2D_RECT), NC(GL_IMAGE_CUBE),
  NC(GL_IMAGE_BUFFER), NC(GL_IMAGE_1D_ARRAY), NC(GL_IMAGE_2D_ARRAY),
  NC(GL_IMAGE_CUBE_MAP_ARRAY), NC(GL_IMAGE_2D_MULTISAMPLE),
  NC(GL_IMAGE_2D_MULTISAMPLE_ARRAY), NC(GL_INT_IMAGE_1D), NC(GL_INT_IMAGE_2D),
  NC(GL_INT_IMAGE_3D), NC(GL_INT_IMAGE_2D_RECT), NC(GL_INT_IMAGE_CUBE),
  NC(GL_INT_IMAGE_BUFFER), NC(GL_INT_IMAGE_1D_ARRAY),
  NC(GL_INT_IMAGE_2D_ARRAY), NC(GL_INT_IMAGE_CUBE_MAP_ARRAY),
  NC(GL_INT_IMAGE_2D_MULTISAMPLE), NC(GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY),
  NC(GL_UNSIGNED_INT_IMAGE_1D), NC(GL_UNSIGNED_INT_IMAGE_2D),
  NC(GL_UNSIGNED_INT_IMAGE_3D), NC(GL_UNSIGNED_INT_IMAGE_2D_RECT),
  NC(GL_UNSIGNED_INT_IMAGE_CUBE), NC(GL_UNSIGNED_INT_IMAGE_BUFFER),
  NC(GL_UNSIGNED_INT_IMAGE_1D_ARRAY), NC(GL_UNSIGNED_INT_IMAGE_2D_ARRAY),
  NC(GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY),
  NC(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE),
  NC(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) };

const GLSLTypeInfo &PGUPV::getGLSLTypeInfo(GLenum type) {
  for (uint i = 0; i < sizeof(glslTypeInfo) / sizeof(glslTypeInfo[0]); i++) {
    if (glslTypeInfo[i].glEnum == type)
      return glslTypeInfo[i];
  }
  return unknownGLSLType;
}

std::string PGUPV::formatSingleGLSLValue(const GLSLTypeInfo &t, void *ptr,
  bool ubo) {
  std::ostringstream result;

  // For matrix types stored in UBO (we *assume* std140) , we should take into
  // account that a column always uses 4 spaces
  if (ubo && t.nrows < 4 && t.ncols > 1) {
    result << "{";
    for (uint i = 0; i < t.ncols; i++) {
      GLSLTypeInfo t2 = t;
      t2.ncols = 1;
      t2.numComponents = t.nrows;
      result << formatSingleGLSLValue(t2, reinterpret_cast<char *>(ptr)+
        i * t.sizePerComponent * 4,
        false);
    }
    result << "}";
    return result.str();
  }
  result << "[";
  if (t.baseTypeGLEnum == GL_FLOAT) {
    auto vals = (GLfloat *)ptr;
    for (uint i = 0; i < t.numComponents; i++) {
      if (i > 0)
        result << ", ";
      result << std::to_string(vals[i]);
    }
  }
  else if (t.baseTypeGLEnum == GL_INT) {
    auto vals = (GLint *)ptr;
    for (uint i = 0; i < t.numComponents; i++) {
      if (i > 0)
        result << ", ";
      result << std::to_string(vals[i]);
    }
  }
  else if (t.baseTypeGLEnum == GL_UNSIGNED_INT) {
    auto vals = (GLuint *)ptr;
    for (uint i = 0; i < t.numComponents; i++) {
      if (i > 0)
        result << ", ";
      result << std::to_string(vals[i]);
    }
  }
  else if (t.baseTypeGLEnum == GL_BOOL) {
    auto vals = (GLint *)ptr;
    for (uint i = 0; i < t.numComponents; i++) {
      if (i > 0)
        result << ", ";
      result << (vals[i] ? "true" : "false");
    }
  }
  else if (t.baseTypeGLEnum == 0) {
    auto vals = (GLint *)ptr;
    result << std::to_string(vals[0]);
  }
  else {
    result << "Tipo opaco";
  }
  result << "]";
  return result.str();
}


std::string PGUPV::formatGLSLValue(const GLSLTypeInfo &t, uint size, void *ptr,
  bool ubo) {
  std::ostringstream result;
  if (size == 1)
    return formatSingleGLSLValue(t, ptr, ubo);
  else {

    result << "ARRAY[" << size << "] = <";
    for (uint i = 0; i < size; i++) {
      result << formatSingleGLSLValue(t, reinterpret_cast<char *>(ptr)+
        i * t.std140ArrayStride);
      if (i != size - 1)
        result << ", ";
    }
    result << ">";
  }
  return result.str();
}

