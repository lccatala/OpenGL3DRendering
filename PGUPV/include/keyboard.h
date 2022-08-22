#pragma once

#include <list>
#include <memory>
#include <functional>
#include "events.h"
#include "common.h"

namespace PGUPV {
  class Keyboard {
  public:
    /**
    \return true si la tecla se ha pulsado en este frame
    */
    bool isKeyDown(PGUPV::KeyCode k);
    /**
    \return true si la tecla está pulsada
    */
    bool isKeyPressed(PGUPV::KeyCode k);
    /**
    \return true si la tecla se ha soltado en este frame
    */
    bool isKeyUp(PGUPV::KeyCode k);

    Keyboard();
    /* Used for testing */
    Keyboard(KeyboardEventsSource *source, std::function<ulong()> currentFrameFunc);
    ~Keyboard();

  private:
    void onKeyboardEvent(const PGUPV::KeyboardEvent &e);
    struct KeyEvent {
      KeyCode k;
      ulong tIn, tOut;
    };

    std::list<KeyEvent> keyboardState;

    void init();
    std::list<KeyEvent>::iterator findInListAndClean(KeyCode keycode);

    KeyboardEventsSource::SubscriptionId subscriptionId;

    KeyboardEventsSource *eventSource;
    std::function<ulong()> currentFrameFunc;
  };

};
