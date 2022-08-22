#ifndef _CAMERA_HANDLER_H
#define _CAMERA_HANDLER_H

#include <glm/glm.hpp>
#include <memory>

#include "events.h"
#include "camera.h"

namespace PGUPV {

  using std::istream;
  using std::ostream;


  class CameraHandler {
  public:
    CameraHandler() : camera(std::make_shared<Camera>()) {};
    virtual ~CameraHandler() {};

    /**
    Devuelve la cámara en su estado actual. No se debería modificar el
    estado de la cámara directamente, ya que el manejador de cámara perdería
    la pista de su estado
    */
    const Camera &getCamera();

    std::shared_ptr<const Camera> getCameraPtr() const { return camera; }
    
    // Reinicia la posición y orientación de la cámara
    virtual void resetView() = 0;
    /**
    En caso de que la cámara haya capturado el ratón, liberarlo
    (muestra el puntero, y la cámara deja de funcionar con el ratón (las teclas
    siguen funcionando)
    */
    virtual void releaseMouse() {};
    /**
    Si la cámara puede capturar el ratón, lo captura (el puntero deja de ser
    visible) para controlar la dirección de la vista.
    */
    virtual void captureMouse() {};
    /**
    \return true si la cámara ha capturado el ratón.
    */
    virtual bool isMouseCaptured() { return false; };

    // Funciones opcionales:
    // deben devolver true si han consumido el evento, y false en otro caso
    // Eventos relacionados con los botones del ratón.
    virtual bool mouse_button(const MouseButtonEvent &) { return false; };
    // Eventos relacionados con el movimiento del ratón
    virtual bool mouse_move(const MouseMotionEvent &) { return false; };
    // Eventos relacionados con la ruleta del ratón
    virtual bool mouse_wheel(const MouseWheelEvent &) { return false; };
    // Eventos de pulsación/liberación de teclas
    virtual bool keyboard(const KeyboardEvent &) { return false; };
    // Evento de paso del tiempo
    virtual void update(uint) {};
    // Eventos de movimiento del joystick
    virtual bool joystick(const JoystickMotionEvent &) { return false; };
    // Eventos de movimiento del hat del gamepad
    virtual bool joystick_hat(const JoystickHatMotionEvent &) { return false; };
    // Eventos de botones del joystick
    virtual bool joystick_button(const JoystickButtonEvent &) { return false; };

    /**
    Evento de cambio de tamaño de ventana
    \param width nuevo ancho de la ventana, en píxeles
    \param height nuevo alto de la ventana, en píxeles
    La implementación de la clase CameraHandler crea por defecto una cámara perspectiva
    de 60º de fov vertical, 0.1 near y 100 far. Si quieres usar un manejador pero
    otra definición de cámara, crea una subclase del manejador, y reescribe este método
    para definir tu volumen de cámara.
    */
    virtual void resized(uint width, uint height);

    /**
    Guarda el estado actual de la cámara en el flujo indicado
    */
    virtual void saveStatus(std::ostream &stream) = 0;
    /**
    restaura el estado de la cámara al que se encuentra en el flujo
    */
    virtual void loadStatus(std::istream &stream) = 0;
  protected:
    std::shared_ptr<Camera> camera;
  };

  /* Cámara que se mueve alrededor de un centro (normalmente el origen) */
  class OrbitCameraHandler : public CameraHandler {
  public:
    // Distancia al origen, y longitud y latitud iniciales (en radianes)
    OrbitCameraHandler(float distanceToOrigin = 3.0f, float initLongitude = 0.0f,
      float initLatitude = 0.0f);
    // Eventos relacionados con el movimiento del ratón
    bool mouse_move(const MouseMotionEvent &e) override;
    bool mouse_wheel(const MouseWheelEvent &e) override;
    bool mouse_button(const MouseButtonEvent &) override { return true; }
    // Eventos de pulsación/liberación de teclas
    virtual bool keyboard(const KeyboardEvent &e);
    void move(float distance, float longitude, float latitude);
    void moveRelative(float ddelta, float longDelta, float latDelta);
    void pan(float dright, float dup);
    void resetView();
    /**
    \return latitud actual de la posición de la cámara, en radianes
    */
    float getLatitude() const { return _lat; };
    /**
    \return longitud actual de la posición de la cámara, en radianes
    */
    float getLongitude() const { return _long; };
    /**
    Guarda el estado actual de la cámara en el flujo indicado
    */
    void saveStatus(std::ostream &stream);
    /**
    restaura el estado de la cámara al que se encuentra en el flujo
    */
    void loadStatus(std::istream &stream);
	/**
	Establece el punto de interés
	*/
	void setPointOfInterest(const glm::vec3 &poi);
	/**
	Establece la matriz de proyección
	*/
	void setProjMatrix(const glm::mat4 &matrix);
  protected:
    void updateMatrix();
    float _initd, _d, _initlong, _long, _initlat, _lat;
    glm::vec3 _initcenter, _center;
  };

  /**
  \class XYPanZoomCamera

  Cámara que siempre mira perpendicularmente al plano XY. El botón izquierdo y el
  central hacen pan y el derecho hace zoom (cambiando el tamaño del volumen de la
  vista).

  \warning Este manejador de cámara puede cambiar el volumen de la cámara en cualquier
  momento, por lo que deberías actualizar la PROJ_MATRIX de GLMatrices a menudo.
  */

#define DISTANCE_TO_PLANE 10.0f

  class XYPanZoomCamera : public CameraHandler {
  public:
    explicit XYPanZoomCamera(float width, glm::vec2 center = glm::vec2(0.0f, 0.0f));
    bool mouse_move(const MouseMotionEvent &e) override;
    bool mouse_wheel(const MouseWheelEvent &e) override;
    bool mouse_button(const MouseButtonEvent &) override { return true; }
    void resetView();
    void resized(uint width, uint height);
    void setWheelSpeed(float speed) { _wheel_speed = speed; };
    float getWheelSpeed() { return _wheel_speed; };
    /**
    Guarda el estado actual de la cámara en el flujo indicado
    */
    void saveStatus(std::ostream &stream);
    /**
    restaura el estado de la cámara al que se encuentra en el flujo
    */
    void loadStatus(std::istream &stream);
  private:
    float _initWidth, _width, _aspectRatio;
    glm::vec2 _initCenter, _center;
    float _wheel_speed;
    void updateProj();
    void updateView();
  };

  /*
  Cámara que permite al usuario "andar" por un plano. Permite ajustar
  la altura inicial de la cabeza, la posición en mundo y la orientación de
  la vista. Los controles por defecto son:

  W, A, S, D: movimiento de la cámara por el terreno (adelante, paso
  lateral
  hacia la izquierda, atrás y paso lateral a la derecha)
  ruleta del ratón: subir/bajar la altura de la cámara
  movimiento del ratón: de izquierda a derecha gira la cámara con respecto a
  la vertical (yaw), de arriba a abajo la gira con respecto a la horizontal
  (pitch).

  */

  class WalkCameraHandler : public CameraHandler {
  public:
    /** Constructor por defecto. Permite establecer el estado inicial
     de la cámara:
     \param h altura de la cámara
     \param pos posición de la cabeza dentro del mundo
     \param yaw ángulo de giro (en radianes) de la cabeza con respecto a la
     vertical (eje Y).
     La dirección yaw = 0 es el eje -Z, y la rotación es dextrógira
     \param pitch: ángulo de giro (en radianes) de la cámara con respecto a la
     horizontal
     La dirección de la vista con pitch = 0 es vista paralela al suelo,
     y la rotación es dextrógira (ángulos positivos levantan la cabeza
     y negativos la bajan).
     */
    WalkCameraHandler(float h = 1.8f,
      const glm::vec4 &pos = glm::vec4(0.0f, 1.8f, 0.0f, 1.0f),
      float yaw = 0.0f, float pitch = 0.0f);
    ~WalkCameraHandler();

    /**
    Libera el ratón (muestra el puntero, y la cámara deja de funcionar con el
    ratón (las teclas siguen funcionando)
    */
    void releaseMouse();
    /**
    Captura el ratón (el puntero deja de ser visible) para controlar la dirección
    de la vista.
    */
    void captureMouse();
    /**
    \return true si la cámara ha capturado el ratón.
    */
    bool isMouseCaptured();

    /**
    Establece las velocidades de actualización de la posición/orientación
    de la cámara:
    \param deltah velocidad con la que cambia la altura con la ruleta del ratón
    \param deltayaw porcentaje de la ventana que hay que recorrer con el
    puntero del ratón  para recorrer 90º. Valor entre 0 y 1. (por defecto 0.4)
    \param deltapitch: ídem para el pitch
    */
    void setSpeeds(float deltah, float deltayaw, float deltapitch);
    /**
    Establece la velocidad vertical de la cámara
    \param deltah: cuánto cambia la altura de la cámara en cada avance de la
    ruleta
    (por defecto 0.1)
    */
    void setVerticalSpeed(float deltah) { _deltah = deltah; };
    /**
    Establece la velocidad horizontal de la cámara
    \param walkSpeed: velocidad (en unidades por segundo) a la que avanza la
    cámara
    (por defecto 1 u/s)
    */
    void setWalkSpeed(float walkSpeed) { _walkSpeed = walkSpeed; };
    // Establece la altura de la cámara
    void setHeight(float height);
    // Establece la posición de la cámara
    void setPos(const glm::vec4 &pos);
    /**
    Establece los ángulos de la dirección de la vista (ver definición
    en el constructor)
    */
    void setAngles(float pitch, float yaw);

    // Funciones de consulta
    float getHeight() { return _h; };
    float getYaw() { return _yaw; };
    float getPitch() { return _pitch; };

    // Eventos relacionados con el movimiento del ratón
    bool mouse_move(const MouseMotionEvent &e) override;
    bool mouse_wheel(const MouseWheelEvent &e) override;
    bool mouse_button(const MouseButtonEvent &) override { return true; }
    void update(uint ms) override;
    void resetView();
    /**
    Guarda el estado actual de la cámara en el flujo indicado
    */
    void saveStatus(std::ostream &stream);
    /**
    restaura el estado de la cámara al que se encuentra en el flujo
    */
    void loadStatus(std::istream &stream);
  private:
    void updateMatrix();
    // Advance the position forward as if has passed 'delta' ms
    // delta can also be negative for going backwards
    void advance(float delta);
    float _inith, _h, _inityaw, _yaw, _initpitch, _pitch;
    float _deltah, _walkSpeed, _deltayaw, _deltapitch;
    glm::vec4 _initpos, _pos;
    bool mouseCaptured;
  };
};

#endif
