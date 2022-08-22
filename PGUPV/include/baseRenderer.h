#ifndef _BASERENDERER_H
#define _BASERENDERER_H 2014

#include <vector>
#include <memory>
#include <cstddef>  // for size_t
#include <string>   // for string

#include "common.h"

namespace PGUPV {

  class CameraHandler;
  class Camera;
  class Panel;

  /*
  Esta clase contiene la funcionalidad común a todos los renderer que no tiene que implementar el usuario de Renderer.
  */
  class BaseRenderer {
  public:
    BaseRenderer() :
      useInspectionCameraFlag(false) 
    {};

    virtual ~BaseRenderer() {};
    /**
    Esta función permite establecer distintos tipos de control de cámara
    */
    void setCameraHandler(std::shared_ptr<CameraHandler> c) {
      userCameraHandler = c;
    }
    /**
    Devuelve el manejador de la cámara
    */
    std::shared_ptr<CameraHandler> getCameraHandler() const;
    /**
    Devuelve la cámara que está gestionando el manejador de cámara (y, por lo tanto, debe existir uno).
    No deberías modificar el estado de la cámara directamente (usa el manejador)
    Si no usas un manejador, define tu propia cámara.
    */
    const Camera& getCamera() const;

    /**
    Operaciones de dibujado que se realizan antes de que el usuario haya dibujado su escena.
    */
    virtual void preRender() {};
    
    /**
    Operaciones de dibujado que se realizan después de que el usuario haya dibujado su escena.
    Si la sobreescribes, recuerda llamar a este método de BaseRenderer
    */
    virtual void postRender();

    /**
    Usa una segunda cámara para dibujar la escena. Se mostrará el frustum de la
    cámara del usuario
    */
    void useInspectionCamera(bool use = true);

    /**
    \return true si se está usando la cámara de inspección
    */
    bool isUsingInspectionCamera() const { return useInspectionCameraFlag; };

    void update_camera(uint ms);

    /**
    Crea un nuevo panel y devuelve un puntero inteligente. No hace falta que lo guardes,
    siempre puedes volver a perdirlo con getPanel(idx)
    \param title Título del panel
    \return Un puntero inteligente al panel recién creado
    */
    std::shared_ptr<Panel> addPanel(const std::string &title);

    /**
    Devuelve el panel que se creó en el orden indicado
    \param index Número de orden en el que se creó el panel que se pide
    \return Un puntero inteligente al panel pedido
    */
    inline std::shared_ptr<Panel> getPanel(size_t index) { return panels[index]; }

    /**
    \return el número de paneles
    */
    size_t getNumPanels() { return panels.size(); };

    /**
    Sobreescribe el siguiente método para almacenar el estado de la aplicación cuando
    el usuario pulse el atajo correspondiente (por ejemplo, Ctrl+F1). La cámara se almacena
    automáticamente, pero tú tendrás que almacenar cualquier otra variable que necesites
    para restaurar el estado a este punto.
    \param stream flujo donde almacenar el estado
    */
    virtual void saveStatus(std::ofstream &) {};

    /**
    Sobreescribe el siguiente método para restaurar el estado de la aplicación a partir
    de los datos almacenados en el flujo (por la función BaseRenderer::saveStatus)
    \param stream flujo desde donde cargar el estado
    */
    virtual void loadStatus(std::ifstream &) {};

    /**
    \return true si la coordenada de ventana indicada está en la región del renderer
    \warning Para los renderer a pantalla completa, siempre devuelve true. Los renderer de 
    tipo ViewportRenderer, devuelven true sólo si la coordenada está dentro del viewport.
    */
    virtual bool isInViewport(uint , uint ) { return true; }
    /**
    Establece un identificador (opcional)
    \param s identificador del renderer
    */
    void setId(const std::string &s) { id = s; }

    /**
    \return El identificador del renderer (puede estar vacío, si no se ha establecido antes)
    */
    const std::string &getId() const { return id; }

  protected:
    std::shared_ptr<CameraHandler> userCameraHandler, inspectionCameraHandler;
    bool useInspectionCameraFlag;
    std::vector<std::shared_ptr<Panel>> panels;
    std::string id;
  };

}; // namespace
#endif
