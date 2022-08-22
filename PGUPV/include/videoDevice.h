#pragma once

#include <vector>
#include <string>
#include <map>

#include "media.h"

namespace media {
	class VideoDevice : public Media {
	public:
		// Abre la cámara con la configuración indicada
		VideoDevice(unsigned int camId, unsigned int optsId = 0, float fps = MAX_FPS);
		// Abre la cámara indicada con las opciones indicadas
		VideoDevice(unsigned int index, const std::string &pixfmtCodec, const std::string &size, float fps);
        ~VideoDevice();
		/**
		Devuelve una lista con el nombre de las cámaras disponibles
		\warning Ahora mismo llama al ejecutable de ffmpeg para obtener el listado, porque la API de 
		libavdevice no devuelve los dispositivos de dshow. No funciona en Linux de momento.
		https://stackoverflow.com/questions/51991436/how-to-get-directshow-device-list-with-ffmpeg-in-c
		*/
		static const std::vector<std::string> getAvailableCameras();

		struct Range {
			std::string size;
			float minfps, maxfps;
		};

		typedef std::multimap<std::string, Range> Options;
		// Devuelve las opciones disponibles para configurar la cámara indicada
		static Options listOptions(unsigned int camera);
		// Constante que indica usar la velocidad mayor
		static const float MAX_FPS;
	private:
		static bool libavInitialized;
		static std::vector<std::string> availableCameras;
		static void initializeLibAv();
		static std::vector<Options> cameraOptions;

		void init();

		void openDevice(unsigned int index);
        // Requested parameters
        float fps_req;
		std::string size_req;
        std::string pixfmtCodec_req;
		unsigned int camera_index;

	};
};
