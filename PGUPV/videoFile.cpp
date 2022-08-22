
extern "C" {
#include <libavformat/avformat.h>
}

#include "log.h"
#include "videoFile.h"

using media::VideoFile;

VideoFile::VideoFile(const std::string &filepath) : filepath(filepath) {

	// Open video file
	if (avformat_open_input(&pFormatCtx, filepath.c_str(), NULL, NULL) != 0)
		ERRT("No se ha podido abrir el fichero " + filepath);

	if (!searchAudioVideoStreams())
		ERRT(filepath + ": No se reconoce el formato del fichero o no contiene una pista de vídeo");

	prepareForReading();
    
    INFO("VideoFile creado (" + filepath + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

VideoFile::~VideoFile() {
    INFO("VideoFile destruido (" + filepath + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

void VideoFile::rewind() {

	int err = av_seek_frame(pFormatCtx, firstVideoStream, pFormatCtx->start_time, AVSEEK_FLAG_BACKWARD); 
	if (err < 0) {
		ERR("No se ha podido saltar al principio del vídeo " + filepath + " (" + ffmpegError(err) + ")");
	}
	else {
		avcodec_flush_buffers(pCodecCtx);
	}
}