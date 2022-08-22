extern "C"
{
#ifdef _WIN32
#pragma warning( push)
#pragma warning( disable : 4244)
#endif
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavutil/dict.h>
#ifdef _WIN32
#pragma warning(pop)
#endif
}
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

#include "app.h"
#include "videoDevice.h"
#include "log.h"
#include "utils.h"


using media::VideoDevice;
using PGUPV::App;


bool VideoDevice::libavInitialized = false;
std::vector<std::string> VideoDevice::availableCameras;

const float VideoDevice::MAX_FPS = 1e6f;

VideoDevice::VideoDevice(unsigned int camId, unsigned int optsId, float fps) :
	camera_index(camId) {
	auto opts = listOptions(camId);
	
	auto opt = opts.begin();
	for (uint i = 0; opt != opts.end() && i < optsId; i++) {
		opt++;
	}
	if (opt == opts.end())
		ERRT("La opción de la cámara no existe");

	pixfmtCodec_req = opt->first;
	size_req = opt->second.size;
	fps_req = fps == MAX_FPS ? opt->second.maxfps : fps;
	init();
    INFO("VideoDevice creado (cámara " + std::to_string(camId) + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}


VideoDevice::VideoDevice(unsigned int index, const std::string & pixfmtCodec, const std::string & size, float fps)
	:fps_req(fps), size_req(size), pixfmtCodec_req(pixfmtCodec), camera_index(index)
{
	init();
    INFO("VideoDevice creado (cámara " + std::to_string(index) + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

VideoDevice::~VideoDevice() {
  INFO("VideoDevice destruído " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void VideoDevice::init() {
	if (!libavInitialized) {
		initializeLibAv();
	}
	openDevice(camera_index);
	if (!searchAudioVideoStreams())
		ERRT("No se ha encontrado el flujo de vídeo de la cámara");
	prepareForReading();
	INFO("Cámara preparada para capturar");
}

#ifdef _WIN32

void runFFMPEG(const std::string &params, const std::string &outputFile) {
	if (!PGUPV::fileExists("../bin/ffmpeg.exe")) {
		ERRT("No se ha encontrado el ejecutable ffmpeg.exe en el directorio bin");
	}

	auto cwd = PGUPV::getCurrentWorkingDir();
	const std::string tmpFile = cwd + outputFile;

	PGUPV::changeCurrentDir("../bin");
	auto res = system(("ffmpeg  " + params + " 2> " + tmpFile).c_str());
	PGUPV::changeCurrentDir(cwd);

	if (res != 1) {
		ERRT("Error al ejecutar ffmpeg");
	}
}

std::multimap<std::string, VideoDevice::Range> VideoDevice::listOptions(unsigned int camera) {
	const std::string tmpFileName = "_ffmpeg.txt";

	if (!libavInitialized) {
		initializeLibAv();
	}

	runFFMPEG("-f dshow -list_options true -i video=\"" +
		availableCameras[camera] + "\"", tmpFileName);

	std::ifstream f(tmpFileName);
	bool recording = false;
	std::multimap<std::string, VideoDevice::Range> options;
	std::string prev_string;

	while (f) {
		std::string s;
		std::getline(f, s);
		std::string codecPixFmt;
		Range r;
		if (recording) {
			if (s.find("Immediate exit requested") != std::string::npos) {
				recording = false;
				break;
			}
			else if (s == prev_string || s.find("alternative pin name") != std::string::npos) {
				continue;
			}
			else
			{
				// s contiene una línea con información, del tipo:
				//  [dshow @ 0333f660]   pixel_format=yuyv422 min s=2304x1536 fps=2 max s=2304x1536 fps=2
				//  [dshow @ 0333f660]   vcodec=h264  min s=640x480 fps=5 max s=640x480 fps=30

				// codec/pixel_format
				auto i = s.find("]");
				if (i == std::string::npos) continue;
				i = s.find_first_not_of(' ', i + 1);
				auto j = s.find("min", i);
				codecPixFmt = s.substr(i, j - i - 2);

				// size
				i = s.find("s=", i);
				if (i == std::string::npos) continue;
				i += 2;
				j = s.find(" ", i);
				r.size = s.substr(i, j - i);

				// min fps
				i = s.find("fps=");
				if (i == std::string::npos) continue;
				i += 4;
				j = s.find(" ", i);
				r.minfps = std::stof(s.substr(i, j - i));

				// repeat size
				i = s.find("s=", i);
				if (i == std::string::npos) continue;
				i += 2;
				j = s.find(" ", i);
				auto tmp = s.substr(i, j - i);

				if (tmp != r.size) {
					ERR("No se esperaba en la misma línea dos tamaños distintos " + s);
					continue;
				}

				// max fps
				i = s.find("fps=", i);
				if (i == std::string::npos) continue;
				i += 4;
				j = s.find(" ", i);
				r.maxfps = std::stof(s.substr(i, j - i));


				prev_string = s;
				options.insert(std::pair<std::string, VideoDevice::Range >(codecPixFmt, r));
			}
		}
		else {
			if (s.find("DirectShow video device options") != std::string::npos)
				recording = true;
		}
	}

	return options;
}

void findAvailableCameras(std::vector<std::string> &list) {
	const std::string tmpFileName = "_ffmpeg.txt";

	runFFMPEG("-f dshow -list_devices true -i dummy", tmpFileName);

	std::ifstream f(tmpFileName);
	bool recording = false;
	list.clear();
	while (f) {
		std::string s;
		std::getline(f, s);
		if (recording) {
			if (s.find("DirectShow audio devices") != std::string::npos) {
				recording = false;
				break;
			}
			else {
				if (s.find("none found") != std::string::npos) break;
				auto b = s.find_last_of("[dshow");
				if (b != std::string::npos && s.find("Alternative name") == std::string::npos) {
					auto fq = s.find("\"");
					auto lq = s.find_last_of("\"");
					if (fq == std::string::npos || lq == std::string::npos) {
						ERRT("La lista de cámaras no está en el formato esperado");
					}
					fq++;
					list.push_back(s.substr(fq, lq - fq));
				}
			}
		}
		if (s.find("DirectShow video devices") != std::string::npos)
			recording = true;
	}

}
#elif __APPLE__
void findAvailableCameras(std::vector<std::string> &list) {
	const std::string tmpFile = "_ffmpeg.txt";
    
    std::string ffmpegPath;
    if (!App::getInstance().getProperty(App::PROP_FFMPEG_EXEC_PATH, ffmpegPath))
        ffmpegPath = "/usr/local/bin/ffmpeg";
    
    int res = system((ffmpegPath + " -f avfoundation -list_devices true -i \"\" 2> " + tmpFile).c_str());
    if (!WIFEXITED(res)) {
		ERRT("No se ha encontrado la librería ffmpeg. Por favor, instálala, o indica su ruta en el fichero de propiedades"
             ", por ejemplo: ffmpeg_exec_path=/usr/local/bin/ffmpeg");
	}
	std::ifstream f(tmpFile);
	bool recording = false;
	list.clear();
	while (f) {
		std::string s;
		std::getline(f, s);
		if (recording) {
			if (s.find("AVFoundation audio devices:") != std::string::npos) {
				recording = false;
				break;
			}
			else {
                // Ignoramos la cámara virtual de captura de la pantalla
                if (s.find("Capture screen") != std::string::npos) continue;
				auto b = s.find_last_of("]");
				if (b != std::string::npos) {
					list.push_back(s.substr(b + 2));
				}
			}
		}
		if (s.find("AVFoundation video devices:") != std::string::npos)
			recording = true;
        else if (s.find("command not found") != std::string::npos) {
            ERRT("No se ha encontrado la librería ffmpeg. Por favor, instálala, o indica su ruta en el fichero de propiedades"
                 ", por ejemplo: ffmpeg_exec_path=/usr/local/bin/ffmpeg");
        }
	}
}



std::multimap<std::string, VideoDevice::Range> VideoDevice::listOptions(unsigned int camera) {
    
    // En MAC estamos devolviendo a piñón un formato estándar, indpendientemente de la cámara.
    // Si alguien se anima a obtener los modos de la cámara, tal y como se hace en Windows, que contacte conmigo
    
    std::multimap<std::string, VideoDevice::Range> result;
    VideoDevice::Range r;
    r.minfps = r.maxfps = 30;
    r.size = "640x480";
    result.insert(std::pair<std::string, VideoDevice::Range>("pixel_format=uyvy422", r));
    return result;
}


#else // Linux
// http://stackoverflow.com/questions/4290834/how-to-get-a-list-of-video-capture-devices-web-cameras-on-linux-ubuntu-c
void findAvailableCameras(std::vector<std::string> &/*list*/) {

	ERRT("Funcionalidad en desarrollo. Habla con Paco");

    /*
	auto l = listLinks("/sys/class/video4linux", false);
	list.clear();
	for (auto c : l) {
		std::ifstream  name(c + "/name");
		if (!name)
			list.push_back(c);
		else {
			std::string cameraName;
			std::getline(name, cameraName);
			list.push_back(cameraName);
		}
	}
     */
}


std::multimap<std::string, VideoDevice::Range> VideoDevice::listOptions(unsigned int /*camera*/) {

	ERRT("Funcionalidad en desarrollo. Habla con Paco");

    std::multimap<std::string, VideoDevice::Range> options;
    
    /*
	const std::string tmpFileName = "_ffmpeg.txt";

	std::string cmd("v4l2-ctl -d /dev/video" + std::to_string(camera) + " --list-formats-ext > " + tmpFileName);
	auto res = system(cmd.c_str());

	if (res != 0) {
		ERRT("Error al ejecutar " + cmd);
	}

	std::ifstream f(tmpFileName);
	bool recording = false;

	while (f) {
		std::string s;
		std::getline(f, s);
		std::string codecPixFmt;
		Range r;
		if (recording) {
			if (s.find("Immediate exit requested") != std::string::npos) {
				recording = false;
				break;
			}
			else
			{
				// s contiene una línea con información, del tipo:
				// [video4linux2, v4l2 @ 0xf09700] Raw       :     yuyv422:           YUYV 4 : 2 : 2 : 640x480 1280x720 960x544 800x448 640x360 424x240 352x288 320x240 800x600 176x144 160x120 1280x800
				// [video4linux2, v4l2 @ 0xf09700] Compressed : mjpeg : Motion - JPEG : 640x480 1280x720 960x544 800x448 640x360 800x600 416x240 352x288 176x144 320x240 160x120
				// /dev/video0 : Immediate exit requested

				// codec/pixel_format
				auto i = s.find("]");
				if (i == std::string::npos) continue;
				i = s.find_first_not_of(' ', i + 1);
				auto j = s.find(" ", i);
				auto type = s.substr(i, j - i);

				i = s.find(":", i);
				j = s.find(":", i + 1);
				auto tmp = s.substr(i + 1, j - i - 1);
				tmp = trim(tmp);

				if (type == "Raw")
					codecPixFmt = std::string("")

					i = s.find_last_of(':');
				i += 2;

				std::istringstream is(s.substr(i));
				while (is) {
					std::string size;
					is >> size;
					options.insert(std::pair<std::string, VideoDevice::Range>());
				}


				// size
				i = s.find("s=", i);
				if (i == std::string::npos) continue;
				i += 2;
				j = s.find(" ", i);
				r.size = s.substr(i, j - i);

				// min fps
				i = s.find("fps=");
				if (i == std::string::npos) continue;
				i += 4;
				j = s.find(" ", i);
				r.minfps = strToFloat(s.substr(i, j - i));

				// repeat size
				i = s.find("s=", i);
				if (i == std::string::npos) continue;
				i += 2;
				j = s.find(" ", i);
				auto tmp = s.substr(i, j - i);

				if (tmp != r.size) {
					ERR("No se esperaba en la misma línea dos tamaños distintos " + s);
					continue;
				}

				// max fps
				i = s.find("fps=", i);
				if (i == std::string::npos) continue;
				i += 4;
				j = s.find(" ", i);
				r.maxfps = strToFloat(s.substr(i, j - i));


				options.insert(std::pair<std::string, VideoDevice::Range >(codecPixFmt, r));
			}
		}
		else {
			if (s.find("[video4linux2,") != std::string::npos)
				recording = true;
		}
	}
*/
	return options;
}

#endif


const std::vector<std::string> media::VideoDevice::getAvailableCameras()
{
	if (!libavInitialized) {
		initializeLibAv();
	}
	return availableCameras;
}

void VideoDevice::initializeLibAv() {
	if (App::getInstance().isIgnoreCamerasSet())
		return;
	avdevice_register_all();
	findAvailableCameras(availableCameras);
	libavInitialized = true;
}



void VideoDevice::openDevice(unsigned int index) {

	AVDictionary *options = nullptr;
	av_dict_set(&options, "framerate", std::to_string(fps_req).c_str(), 0);
	av_dict_set(&options, "video_size", size_req.c_str(), 0);
	auto eqIdx = pixfmtCodec_req.find("=");
	if (eqIdx == std::string::npos)
		ERRT("Opción no reconocida: " + pixfmtCodec_req);
	auto var = pixfmtCodec_req.substr(0, eqIdx);
	auto val = pixfmtCodec_req.substr(eqIdx + 1);
	av_dict_set(&options, var.c_str(), val.c_str(), 0);


#ifdef _WIN32
	if (index >= availableCameras.size())
		ERRT("Esa cámara no existe");

	// First, we will try to use dshow
	AVInputFormat *ifmt = av_find_input_format("dshow");
	//Set own video device's name
	if (avformat_open_input(&pFormatCtx, ("video=" + availableCameras[index]).c_str(), ifmt, &options) != 0) {

		// If not found by dshow, try with the older VFW
		ifmt = av_find_input_format("vfwcap");
		if (avformat_open_input(&pFormatCtx, std::to_string(index).c_str(), ifmt, &options) != 0) {
			ERRT("No se ha podido abrir la cámara " + availableCameras[index]);
		}
	}
	INFO("Cámara " + availableCameras[index] + " abierta correctamente");
#elif __linux__
	if (index >= availableCameras.size())
		ERRT("Esa cámara no existe");
	//Linux
	AVInputFormat *ifmt = av_find_input_format("video4linux2");
	if (avformat_open_input(&pFormatCtx, availableCameras[index].c_str(), ifmt, &options) != 0) {
		ERRT("No se ha podido abrir la cámara " + availableCameras[index]);
	}
	INFO("Cámara " + availableCameras[index] + " abierta correctamente");

#else
	//Mac
	AVInputFormat *ifmt = av_find_input_format("avfoundation");
	//Avfoundation
	//[video]:[audio]
	if (avformat_open_input(&pFormatCtx, std::to_string(index).c_str(), ifmt, &options) != 0) {
		ERRT("No se ha podido abrir la cámara " + std::to_string(index));
	}
	INFO("Cámara " + std::to_string(index) + " abierta correctamente");

#endif
	// Ignored options
	AVDictionaryEntry *t = nullptr;
	while ((t = av_dict_get(options, "", t, AV_DICT_IGNORE_SUFFIX)) != nullptr) {

		WARN(std::string("Opción no reconocida por la cámara: ") + t->key + "=" + t->value);
	}
	av_dict_free(&options);


}
