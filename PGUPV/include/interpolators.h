#ifndef _INTERPOLATORS_H
#define _INTERPOLATORS_H

#include <glm/glm.hpp>
#include <vector>

#include "log.h"


namespace PGUPV {
  /* Un interpolador lineal (en principio, puede interpolar cualquier cosa
  que se pueda restar y que se pueda multiplicar por un escalar).
  */
  enum { ONE_TIME, PING_PONG, REPEAT };
  template <class T> class LinearInterpolator {
  public:
    // Constructor:
    // begin, end: valores inicial y final entre los que interpolar
    // time: duración de un ciclo
    // type: ONE_TIME: sólo un viaje. Cuando llega al final, se queda
    //		REPEAT: infinito, cuando llega al final vuelve al principio
    //		PING_PONG: infinito, va desde el inicio al final y de vuelta
    LinearInterpolator(T begin, T end, double time, int type)
      : _begin(begin), _end(end), _time(time), _type(type) {};
    // Llama a esta función para devolver el valor interpolado en el
    // instante t
    T interpolate(double t);

  private:
    T _begin, _end;
    double _time;
    int _type;
  };

  template <class T> T LinearInterpolator<T>::interpolate(double t) {
    if (t < 0.0 || _time == 0.0)
      return _begin;
    if (t >= _time) {
      if (_type == ONE_TIME)
        return _end;
      if (_type == REPEAT)
        t = fmod(t, _time);
      else {
        t = fmod(t, 2 * _time);
        if (t >= _time)
          t = 2 * _time - t;
      }
    }
    T pos = _begin + (_end - _begin) * ((float)(t / _time));
    return pos;
  }

  template <class T, class V> struct KeyFrame {
    T time;
    V value;
  };

  template <class T, class V> class MultiLinearInterpolator {
  public:
    /** Constructor:
    // begin, end: valores inicial y final entre los que interpolar
    // time: duración de un ciclo
    // type: ONE_TIME: sólo un viaje. Cuando llega al final, se queda
    //		REPEAT: infinito, cuando llega al final vuelve al principio
    //		PING_PONG: infinito, va desde el inicio al final y de vuelta
    */
    MultiLinearInterpolator(const std::vector<KeyFrame<T, V>> &keyframes,
      int type)
      : _keyframes(keyframes), _type(type) {
      if (keyframes.empty())
        ERRT("Al menos debe haber un frame");
    };
    /** Llama a esta función para devolver el valor interpolado en el
    instante t
    */
    V interpolate(T t);

  private:
    std::vector<KeyFrame<T, V>> _keyframes;
    int _type;
  };

  template <class T, class V> V MultiLinearInterpolator<T, V>::interpolate(T t) {
    const T startTime = _keyframes.front().time;
    if (t <= startTime || _keyframes.size() == 1)
      return _keyframes.front().value;

    const T endTime = _keyframes.back().time;
    if (t > endTime) {
      if (_type == ONE_TIME)
        return _keyframes.back().value;
      auto duration = endTime - startTime;
      if (_type == REPEAT)
        t = fmod(t - startTime, duration) + startTime;
      else {
        t = fmod(t - startTime, 2 * duration);
        if (t >= duration)
          t = 2 * duration - t + startTime;
      }
    }

    // starttime < t <= endTime
    size_t i;
    for (i = 1; i < _keyframes.size(); i++) {
      if (_keyframes[i].time >= t)
        break;
    }

    V value = _keyframes[i - 1].value +
      (_keyframes[i].value - _keyframes[i - 1].value) * (t - _keyframes[i - 1].time) /
      (_keyframes[i].time - _keyframes[i - 1].time);
    return value;

  }

  /**
  \class CircularInterpolator

  Interpolador circular
  */
  class CircularInterpolator {
  public:
    /**
  Crea un interpolador circular, según los parámetros indicados. Todos los
  parámetros son opcionales.
    \param center centro de rotación
    \param initpos: posición inicial
    \param axis eje de rotación
    \param time_cycle tiempo en segundos para completar un ciclo
    \param radians_cycle ángulo que se recorre en un ciclo
    \param type ONE_TIME: sólo un viaje. Cuando llega al final, se queda
                    REPEAT: infinito, cuando llega al final vuelve al principio
                    PING_PONG: infinito, va desde el inicio al final y de vuelta
  */
    CircularInterpolator(glm::vec4 center = glm::vec4(0.0f),
      glm::vec4 initpos = glm::vec4(1.0, 0.0, 0.0, 1.0),
      glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0),
      float time_cycle = 2.0, float radians_cycle = 2.0f * 3.14159265f,
      int type = REPEAT);
    // Llama a esta función para devolver el valor interpolado en el
    // instante t
    glm::vec4 interpolate(double t);

  private:
    glm::vec4 _center, _initpos, _endpoint, _initpos2;
    glm::vec3 _axis;
    float _time_cycle, _radians_cycle;
    int _type;
  };
};
#endif
