#pragma once

#include "common.h"

#include <bitset>
#include <memory>
#include <glm/glm.hpp>

#include "events.h"
#include "HW.h"

namespace PGUPV {
  /**
   \class Gamepad
   Esta clase representa un gamepad (o joystick). Un gamepad está compuesto por varias
   palancas (joysticks), palancas digitales (o 'hats') y botones.
   */
  class Gamepad {
  public:
    /**
     Asocia este objeto con el joystick id-ésimo del sistema
     */
    explicit Gamepad(size_t id);
    Gamepad(const Gamepad &) = delete;
    /**
     Obtiene la dirección actual de una palanca del gamepad. Una palanca está compuesta por dos
     ejes. Si el gamepad tiene 4 ejes, el joystick 0 está compuesto por los dos primeros ejes y
     el joystick 1 por los dos últimos ejes.
     La posición de un eje está entre -1 y 1.
     \param joystick número de joystick (un valor entre 0 y (numEjes/2)-1 )
     \return la dirección del joystick entre -1 y 1 en cada dirección
     */
    glm::vec2 getDirection(uint joystick);
    /**
     Obtiene la dirección del primer joystick que no esté en reposo. Así, la aplicación funciona
     con cualquiera de los joysticks
     */
    glm::vec2 getDirection();

    /**
     \return true si el botón indicado está pulsado.
     */
    bool isPressed(uint button);

    /**
    \return true si hay algún botón pulsado
    */
    bool isPressed();
    /**
     Devuelve la posición de un 'hat'. La diferencia entre un joystick y un hat es que el valor
     que puede tener un hat es siempre discreto (entre las 8 direcciones y el estado de reposo)
     */
    HatPosition getHatPosition(uint hat);

    void onJoystickMove(const PGUPV::JoystickMotionEvent &e);
    void onHatMove(const PGUPV::JoystickHatMotionEvent &e);
    void onJoystickButton(const PGUPV::JoystickButtonEvent &e);
  private:
    std::vector<glm::vec2> directions;
    std::vector<HatPosition> hats;
    std::bitset<32> buttons;
    size_t numButtons;
    std::unique_ptr<GamepadHW> gamepadHW;
  };

};
