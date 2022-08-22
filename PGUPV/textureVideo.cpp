#include "textureVideo.h"
#include "videoFile.h"
#include "videoDevice.h"
#include "utils.h"
#include "app.h"
#include "log.h"

using PGUPV::TextureVideo;
using media::Media;
using media::VideoFile;
using media::VideoDevice;


TextureVideo::TextureVideo(const std::string &path) : status(Status::PLAYING), updateCallbackId(static_cast<size_t>(-1)) {
	media = std::unique_ptr<VideoFile>(new VideoFile(path));
  media->setAutoLoop(true);
	init();
	INFO("Nuevo TextureVideo (" + path + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
	_name = PGUPV::getFilenameFromPath(path);
}


TextureVideo::TextureVideo(int camId, int confId, float fps) : status(Status::PLAYING), updateCallbackId(static_cast<size_t>(-1)) {
	media = std::unique_ptr<VideoDevice>(new VideoDevice(camId, confId, fps));
	init();
	INFO("Nuevo TextureVideo (Cámara " + std::to_string(camId) + ") " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

TextureVideo::~TextureVideo() {
	unregisterCallback();
	INFO("TextureVideo destruido " + std::to_string(reinterpret_cast<std::uint64_t>(this)));
}

TextureVideo::TextureVideo(TextureVideo &&other) :
	media(std::move(other.media)), status(other.status)
{
	other.unregisterCallback();
	init();
}

TextureVideo &TextureVideo::operator=(TextureVideo &&other) {
	other.unregisterCallback();
	media = std::move(other.media);
	status = other.status;
	init();
	return *this;
}


void PGUPV::TextureVideo::update()
{
	assert(status == Status::PLAYING);
	auto bytes = media->getNextFrame(true);
	if (bytes != nullptr) {
		bind();
		glTexSubImage2D(_texture_type, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
}

void TextureVideo::init() {
	allocate(media->getWidth(), media->getHeight(), GL_RGBA);
	registerCallback();
	update();
}

void TextureVideo::pause(bool pause) {
	if (pause) {
		unregisterCallback();
		status = Status::PAUSE;
	} else {
		registerCallback();
		status = Status::PLAYING;
	}
}

bool TextureVideo::isPaused() {
	return status == Status::PAUSE;
}

void PGUPV::TextureVideo::rewind()
{
	auto videoFile = dynamic_cast<VideoFile *>(media.get());
	if (videoFile)
		videoFile->rewind();
}

void TextureVideo::registerCallback() {
	if (updateCallbackId != static_cast<size_t>(-1)) {
		unregisterCallback();
	}
	updateCallbackId = App::getInstance().addPostRender([this]() { update(); });
}

void TextureVideo::unregisterCallback() {
	if (updateCallbackId == static_cast<size_t>(-1)) return;
	App::getInstance().removePostRender(updateCallbackId);
	updateCallbackId = static_cast<size_t>(-1);
}

