#pragma once

#include <string>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;

namespace media {
  class Media {
  public:
    Media();
    virtual ~Media();
    //! Frames por segundo (0.0 si no está disponible)
    float getFPS() const;
    //! Ancho del fotograma
    int getWidth() const;
    //! Alto del fotograma
    int getHeight() const;
    //! Descripción del codec
    std::string getCodecDescription() const;
    //! Descripción de la codificación de los píxeles
    std::string getPixelFormat() const;
    //! Devuelve un puntero al siguiente frame (RGB24), o NULL si no hay más
    uint8_t *getNextFrame(bool originAtBottom = true);
    //! Devuelve true si se ha llegado al final del fichero
    bool endOfVideoReached() const { return endOfVideo; }
    void setAutoLoop(bool loop) { autoloop = loop; }
  protected:
    bool searchAudioVideoStreams();
    void prepareForReading();

    int firstVideoStream = -1, firstAudioStream = -1;
    // ffmpeg stuff
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext	*pCodecCtx = nullptr;
    AVFrame         *pFrame = nullptr;
    AVFrame         *pFrameRGB = nullptr;
    uint8_t         *buffer = NULL;
    struct SwsContext      *sws_ctx = nullptr;
    unsigned long msPerFrame; // Duración de un frame en ms (1000/FPS)
  private:
    static bool libInitialized;
    bool autoloop, endOfVideo;

  };

  std::string ffmpegError(int errnum);
};
