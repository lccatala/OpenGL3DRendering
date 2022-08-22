#pragma once

#include <cstdint>

namespace PGUPV {
  /**
  \class StopWatch
  Esta clase permite tomar tiempos de ejecución fácilmente.

  Ejemplo:

  StopWatch reloj;
  ...código a medir...
  cout << reloj.getElapsed();

  ó más complejo:

  StopWatch reloj;
  ...
  reloj.restart();
  ...código a medir...
  int64_t t1 = reloj.getElapsedAndRestart();
  ...código a medir...
  int64_t t2 = reloj.getElapsedAndRestart();
  ...
  reloj.restart();
  ...código a medir...
  int64_t t3 = reloj.getElapsed();
  */
  class StopWatch {
  public:
    /**
    Constructor que además inicia la toma de tiempos (no hace falta llamar a
    restart si se quiere empezar a medir el tiempo ya)
    */
    StopWatch() = default;
    /**
    Devuelve el tiempo transcurridos desde que se construyó el objeto,
    o desde la última llamada a getElapsed o restart, lo que haya ocurrido después.
    \warning Esta llamada resetea el contador
    */
    virtual int64_t getElapsedAndRestart() = 0;
    /**
    Devuelve el tiempo (en nanosegundos) transcurridos desde que se construyó el objeto,
    o desde la última llamada a getElapsedAndRestart o restart, lo que haya ocurrido después.
    */
    virtual int64_t getElapsed() = 0;
    /**
    Resetea el contador a cero
    */
    virtual void restart() = 0;
  };

  class NullStopWatch : public StopWatch {
  public:
    int64_t getElapsedAndRestart() override { return 0; };
    int64_t getElapsed() override { return 0; };
    void restart() override {};
  };

  class MicroSecStopWatch : public StopWatch {
  public:
    MicroSecStopWatch();
    int64_t getElapsedAndRestart() override;
    int64_t getElapsed() override;
    void restart() override;
  private:
    int64_t begin;
  };
};
