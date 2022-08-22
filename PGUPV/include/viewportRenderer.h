
#pragma once

#include "renderer.h"
#include "glStateCache.h"

namespace PGUPV {

  /**
  \class ViewportRenderer

  Es un Renderer, pero con un viewport que no ocupa toda la pantalla. No dibuja nada fuera de su viewport,
  e ignora los eventos que le llegan que ocurren fuera de su viewport (excepto si tiene capturado el
  ratón.

  */
  class ViewportRenderer : public Renderer {
  public:
    /**
    Constructor por defecto. Se define un viewport de 100x100 píxeles, en 0, 0
    */
    ViewportRenderer() : origin(0, 0), size(100, 100), viewport(false), scissor(false)
    {};

    /**
    Constructor con el tamaño del viewport
    \param x coordenada x de la esquina inferior izquierda
    \param y coordenada y de la esquina inferior izquierda
    \param w ancho del viewport
    \param h alto del viewport
    */
    ViewportRenderer(uint x, uint y, uint w, uint h) : origin(x, y), size(w, h), viewport(false), scissor(false)
    {};
    virtual ~ViewportRenderer() { };

    /**
    Cambia el tamaño del viewport
    \param w nuevo ancho
    \param h nuevo alto
    */
    void setSize(uint w, uint h) {
      size.x = w; size.y = h;
    }

    /**
    Cambia la posición del viewport
    \param x coordenada x de la esquina inferior izquierda
    \param y coordenada y de la esquina inferior izquierda
    */
    void setPos(uint x, uint y) {
      origin.x = x; origin.y = y;
    }

    /**
    Establece el viewport
    \param x coordenada x de la esquina inferior izquierda
    \param y coordenada y de la esquina inferior izquierda
    \param w nuevo ancho
    \param h nuevo alto
    */
    void setViewport(uint x, uint y, uint w, uint h) {
      setPos(x, y);
      setSize(w, h);
    }

    virtual void preRender();
    virtual void postRender();

    // En esta función puedes inicializar tu escena, el estado de OpenGL, etc.
    // Sólo se llamará una vez, al principio
    virtual void setup() {};
    // Aquí es donde dibujas los objetos de tu escena, en el estado actual
    virtual void render() = 0;
    // Esta función te indica que han pasado 'ms' milisegundos. Así puedes
    // mover los objetos dinámicos de tu escena dependiendo de su velocidad
    virtual void update(uint) {};
    // Esta función se llamará cuando el usuario cierre la ventana. Incluye aquí el código
    // para liberar los recursos reservados
    virtual void release() {};

    // GESTIÓN DE EVENTOS
    // Funciones a implementar obligatoriamente
    /**
    Esta función se llamará cada vez que la ventana cambie de tamaño.
    Aquí tendrás que recalcular la posición y el tamaño del viewport (la función recibe
    el tamaño de la ventana). Para definir la posición y tamaño del viewport, usa
    ViewportRenderer::setPos y ViewportRenderer::setSize, o ViewportRenderer::setViewport.
    \param w ancho de la ventana en píxeles
    \param h alto de la ventana en píxeles
    */
    virtual void reshape(uint w, uint h) = 0;

    // Funciones opcionales: 
    //		deben devolver true si han consumido el evento, y false en otro caso
    // Eventos relacionados con los botones del ratón.
    virtual bool mouse_button(const MouseButtonEvent &) { return false; };
    // Eventos relacionados con el movimiento del ratón
    virtual bool mouse_move(const MouseMotionEvent &) { return false; };
    // Eventos relacionados con la ruleta del ratón
    virtual bool mouse_wheel(const MouseWheelEvent &) { return false; };
    // Eventos de pulsación/liberación de teclas
    virtual bool keyboard(const KeyboardEvent &) { return false; };
    // Eventos de movimiento del joystick
    virtual bool joystick(const JoystickMotionEvent &) { return false; };
    // Eventos de movimiento del 'hat' del joystick
    virtual bool joystick_hat(const JoystickHatMotionEvent &) { return false; };
    // Eventos de pulsaciones de botones del joystick
    virtual bool joystick_button(const JoystickButtonEvent &) { return false; };

    bool isInViewport(uint x, uint y);
  protected:
    glm::uvec2 origin, size;
    GLStateCapturer<ViewportState> viewport;
    GLStateCapturer<ScissorTestState> scissor;
  };

};
