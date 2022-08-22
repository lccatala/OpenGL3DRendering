#pragma once

#include <memory>
#include "media.h"
#include "texture2D.h"
#include "videoDevice.h"


namespace PGUPV {
  /**
  \class TextureVideo

  Construye una textura 2D asociada a un flujo de vídeo, que puede venir desde una cámara o desde
  un fichero de vídeo.

  */

  class TextureVideo : public Texture2D {
  public:
    /**
    La textura estará asociada al vídeo indicado
    \param path ruta del fichero de vídeo
    */
    TextureVideo(const std::string &path);
    /**
    La textura estará asociada a la cámara indicada
    \param camId identificador de la cámara a utilizar. Se pueden listar las cámaras instaladas
    en el sistema con la opción -listCameras al ejecutar cualquier aplicación PGUPV, o programáticamente
    con media::VideoDevice::getAvailableCameras
    \param confId identificador de la configuración de la cámara a utilizar. La configuración indica el
    codec, el tamaño del vídeo y los frames capturados por segundo. Para listar las opciones de una cámara
    se puede usar la opción -listOpts (cam) en cualquier aplicación PGUPV o programáticamente con
    media::VideoDevice::listOptions
    \param fps indica el número de frames a capturar por segundo.
    */
    TextureVideo(int camId = 0, int confId = 0, float fps = media::VideoDevice::MAX_FPS);
    ~TextureVideo();
    // Prohibimos copia
    TextureVideo(const TextureVideo &) = delete;
    TextureVideo &operator=(const TextureVideo &) = delete;

    TextureVideo(TextureVideo &&other);
    TextureVideo &operator=(TextureVideo &&other);

    /**
    Intenta cargar el siguiente frame
    */
    void update();

    /**
     Pausa/reinicia la reproducción
     \param pause si true, pausa la reproducción, si false, la reinicia
     */
    void pause(bool pause = true);
    /**
     \return true si el vídeo está en pausa
     */
    bool isPaused();
    /**
      Rebobina el vídeo (sólo si la textura está reproduciendo un archivo de vídeo, no
      hace nada en caso de estar asociada a una cámara)
    */
    void rewind();
  private:
    void registerCallback();
    void unregisterCallback();
    std::unique_ptr<media::Media> media;
    void init();
    enum class Status { PLAYING, PAUSE };
    Status status;
    size_t updateCallbackId;
  };
};
