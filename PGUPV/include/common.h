

#ifndef _COMMON_H
#define _COMMON_H

#include <stdexcept>
#include <cstddef>
#ifdef __APPLE__
#ifdef DEBUG
#define _DEBUG
#endif
#endif

#include <string>
#include <vector>
#include <glm/glm.hpp>

// Puntos de vinculación globales para bloques de uniforms.
// Usar estos puntos de vinculación permite que varios shaders compartan un
// mismo U.B.O. (por ejemplo el estado de las matrices model, view, projection,
// etc).
#define UBO_GL_MATRICES_BINDING_INDEX 0
#define UBO_MATERIALS_BINDING_INDEX 1
#define UBO_LIGHTS_BINDING_INDEX 2
#define UBO_BONES_BINDING_INDEX 3
#define UBO_PBR_MATERIALS_BINDING_INDEX 4
#define UBO_PBR_LIGHTS_BINDING_INDEX 5

#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef ushort
typedef unsigned short ushort;
#endif
#ifndef uint
typedef unsigned int uint;
#endif
#ifndef ulong
typedef unsigned long ulong;
#endif

typedef std::vector<std::string> Strings;

namespace PGUPV {
struct Size {
  uint width, height;
  Size() : width(0), height(0){};
  Size(uint w, uint h) : width(w), height(h){};
};

template <typename B> struct TRange {
  typedef B baseType;
  baseType min, max;
  TRange(){};
  TRange(baseType xmin, baseType xmax) : min(xmin), max(xmax){};
};

typedef TRange<std::int32_t> IRange;
typedef TRange<float> Range;

template <typename R> struct TRange2 {
  R lims[2];
  TRange2(typename R::baseType xmin, typename R::baseType xmax,
          typename R::baseType ymin, typename R::baseType ymax) {
    lims[0].min = xmin;
    lims[0].max = xmax;
    lims[1].min = ymin;
    lims[1].max = ymax;
  };
};

template <typename R> struct TRange3 {
  R lims[3];
  TRange3(typename R::baseType xmin, typename R::baseType xmax,
          typename R::baseType ymin, typename R::baseType ymax,
          typename R::baseType zmin, typename R::baseType zmax) {
    lims[0].min = xmin;
    lims[0].max = xmax;
    lims[1].min = ymin;
    lims[1].max = ymax;
    lims[2].min = zmin;
    lims[2].max = zmax;
  };
};

template <typename R> struct TRange4 {
  R lims[4];
  TRange4(typename R::baseType xmin, typename R::baseType xmax,
          typename R::baseType ymin, typename R::baseType ymax,
          typename R::baseType zmin, typename R::baseType zmax,
          typename R::baseType wmin, typename R::baseType wmax) {
    lims[0].min = xmin;
    lims[0].max = xmax;
    lims[1].min = ymin;
    lims[1].max = ymax;
    lims[2].min = zmin;
    lims[2].max = zmax;
    lims[3].min = wmin;
    lims[3].max = wmax;
  };
};

typedef TRange2<IRange> IRange2;
typedef TRange3<IRange> IRange3;
typedef TRange4<IRange> IRange4;

typedef TRange2<Range> Range2;
typedef TRange3<Range> Range3;
typedef TRange4<Range> Range4;

}; // namespace

#define SDELETE(p)                                                             \
  do {                                                                         \
    delete p;                                                                  \
    p = 0;                                                                     \
  } while (0)

#ifndef MIN
template <typename T>
inline T MIN(T a, T b) { 
	if (a < b) 
		return a; 
	else 
		return b; 
}

template <typename T>
inline T MAX(T a, T b) {
	if (a > b)
		return a;
	else
		return b;
}
#endif

#define PIf 3.14159265f
#define HALFPIf (PIf/2.0f)
#define TWOPIf (PIf * 2.0f)

#endif
