#pragma once

#include <cstdint>

namespace PGUPV {
  /**
  \class StopWatch
  Esta clase permite tomar tiempos de ejecuci�n f�cilmente.

  Ejemplo:

  StopWatch reloj;
  ...c�digo a medir...
  cout << reloj.getElapsed();

  � m�s complejo:

  StopWatch reloj;
  ...
  reloj.restart();
  ...c�digo a medir...
  int64_t t1 = reloj.getElapsedAndRestart();
  ...c�digo a medir...
  int64_t t2 = reloj.getElapsedAndRestart();
  ...
  reloj.restart();
  ...c�digo a medir...
  int64_t t3 = reloj.getElapsed();
  */
  class StopWatch {
  public:
    /**
    Constructor que adem�s inicia la toma de tiempos (no hace falta llamar a
    restart si se quiere empezar a medir el tiempo ya)
    */
    StopWatch() = default;
    /**
    Devuelve el tiempo transcurridos desde que se construy� el objeto,
    o desde la �ltima llamada a getElapsed o restart, lo que haya ocurrido despu�s.
    \warning Esta llamada resetea el contador
    */
    virtual int64_t getElapsedAndRestart() = 0;
    /**
    Devuelve el tiempo (en nanosegundos) transcurridos desde que se construy� el objeto,
    o desde la �ltima llamada a getElapsedAndRestart o restart, lo que haya ocurrido despu�s.
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
