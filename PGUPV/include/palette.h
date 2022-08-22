#ifndef _PALETTE_H
#define _PALETTE_H


#include "glm/glm.hpp"

namespace PGUPV {

  struct Palette {
    glm::vec4 activeBg, activeFg;
    glm::vec4 hoverBg, hoverFg;
    glm::vec4 pressedBg, pressedFg;
    glm::vec4 disabledBg, disabledFg;

    /**
    Paleta por defecto. Viene de aquí: http://ui-cloud.com/just-another-ui-kit/
    */
    Palette() {
      activeBg = glm::vec4(255.0f, 106.0f, 55.0f, 255.0f) / 255.0f;
      activeFg = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
      hoverBg = glm::vec4(251.0f, 138.0f, 99.0f, 255.0f) / 255.0f;
      hoverFg = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
      pressedBg = glm::vec4(128.0, 60.0f, 20.0f, 255.0f) / 255.0f;
      pressedFg = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
      disabledBg = glm::vec4(232.0f, 188.0f, 173.0f, 255.0f) / 255.0f;
      disabledFg = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
    }
  };

};

#endif
