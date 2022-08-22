
#include <vector>
#include <assert.h>

extern "C" {
#ifdef _WIN32
#pragma warning( push, 3)
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#ifdef _WIN32
#pragma warning( pop )
#endif
}

#include "app.h"
#include "media.h"
#include "log.h"

using media::Media;

bool Media::libInitialized = false;

Media::Media() : autoloop(false), endOfVideo(false) {
	if (!libInitialized) {
		// Register all formats and codecs
		av_register_all();
	}
}

Media::~Media() {
	// Free the RGB image
	if (buffer) av_free(buffer);

	if (pFrameRGB) av_free(pFrameRGB);

	// Free the YUV frame
	if (pFrame) av_free(pFrame);

	// Close the codec
	if (pCodecCtx) avcodec_close(pCodecCtx);

	if (pCodecCtx) avcodec_free_context(&pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);
}

bool Media::searchAudioVideoStreams() {
	// Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		return false;

	firstVideoStream = firstAudioStream = -1;

	// Find the best candidate for a video stream
	auto i = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (i < 0)
		return false;
	firstVideoStream = i;

	i = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	// Puede ser <0, pero por ahora tampoco nos interesa
	if (i >= 0)
		firstAudioStream = i;

	return true;
}

void Media::prepareForReading() {

	AVCodecParameters *origin_par = pFormatCtx->streams[firstVideoStream]->codecpar;

	// Find the decoder for the video stream
	AVCodec *pCodec = avcodec_find_decoder(origin_par->codec_id);
	if (pCodec == NULL) {
		ERRT("No se ha encontado el codec");
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (pCodecCtx == nullptr) {
		ERRT("Sin memoria");
	}

	if (pCodec->capabilities & AV_CODEC_CAP_TRUNCATED)
		pCodecCtx->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames

	if (avcodec_parameters_to_context(pCodecCtx, origin_par))
		ERRT("Error interno en el decodificador de vídeo");

	// Open codec
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		ERRT("No se ha podido abrir el codec del fichero");

	// Allocate video frame
	pFrame = av_frame_alloc();
	// Allocate an AVFrame structure
	pFrameRGB = av_frame_alloc();
	if (pFrame == nullptr || pFrameRGB == nullptr)
		ERRT("Sin memoria decodificando el vídeo");

	// Determine required buffer size and allocate buffer
	auto numBytes = av_image_get_buffer_size(
		AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);
	if (numBytes <= 0)
		ERRT("Tamaño de vídeo incorrecto ¿Fichero corrupto?");

	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

	auto w = getWidth();
	auto h = getHeight();

	sws_ctx =
		sws_getContext
		(
			w, h,
			pCodecCtx->pix_fmt,
			w, h,
			AV_PIX_FMT_RGB24,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
		);

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
		AV_PIX_FMT_RGB24, w, h, 1);

	msPerFrame = static_cast<unsigned int>(1000.0f / getFPS());
}

float Media::getFPS() const {
	assert(pFormatCtx != nullptr);
	if (firstVideoStream == -1)
		ERRT("No se han encontrado flujos de vídeo");

	auto rational = pFormatCtx->streams[firstVideoStream]->avg_frame_rate;
	if (rational.den == 0) return 0.0f;

	return static_cast<float>(rational.num) / rational.den;
}

int Media::getWidth() const {
	assert(pFormatCtx != nullptr);
	if (firstVideoStream == -1)
		ERRT("No se han encontrado flujos de vídeo");

	return pFormatCtx->streams[firstVideoStream]->codecpar->width;
}

int Media::getHeight() const {
	assert(pFormatCtx != nullptr);
	if (firstVideoStream == -1)
		ERRT("No se han encontrado flujos de vídeo");

	return pFormatCtx->streams[firstVideoStream]->codecpar->height;
}

std::string Media::getCodecDescription() const
{
	assert(pFormatCtx != nullptr);
	if (firstVideoStream == -1)
		ERRT("No se han encontrado flujos de vídeo");

	const AVCodecDescriptor *codec = avcodec_descriptor_get(pFormatCtx->streams[firstVideoStream]->codecpar->codec_id);
	return std::string(codec->name) + " (" + codec->long_name + ")";
}


std::string Media::getPixelFormat() const {
	assert(pFormatCtx != nullptr);
	if (firstVideoStream == -1)
		ERRT("No se han encontrado flujos de vídeo");

	AVPixelFormat pix_fmt = static_cast<AVPixelFormat>(pFormatCtx->streams[firstVideoStream]->codecpar->format);

	return av_get_pix_fmt_name(pix_fmt);
}



// From https://blogs.gentoo.org/lu_zero/2016/03/29/new-avcodec-api/
static int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
	int ret;

	*got_frame = 0;

	if (pkt) {
		ret = avcodec_send_packet(avctx, pkt);
		// In particular, we don't expect AVERROR(EAGAIN), because we read all
		// decoded frames with avcodec_receive_frame() until done.
		if (ret < 0)
			return ret == AVERROR_EOF ? 0 : ret;
	}

	ret = avcodec_receive_frame(avctx, frame);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
		return ret;
	if (ret >= 0)
		*got_frame = 1;

	return 0;
}

uint8_t *Media::getNextFrame(bool originAtBottom) {
	AVPacket packet;
	int frameFinished;
	uint8_t *result = nullptr;
	bool done = false;
	static unsigned long lastFrameTimeStamp = 0;

	auto now = PGUPV::App::getInstance().getCurrentMillis();
	auto delta = now - lastFrameTimeStamp;
	if (delta < msPerFrame)
		return nullptr;

	lastFrameTimeStamp = now;

  int avreadReturnCode = -1;
	while (!done && (avreadReturnCode = av_read_frame(pFormatCtx, &packet)) >= 0) {
		// Is this a packet from the video stream?
		if (packet.stream_index == firstVideoStream) {
			decode(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished) {
				// Convert the image from its native format to RGB
				sws_scale(
					sws_ctx,
					(uint8_t const * const *)pFrame->data,
					pFrame->linesize,
					0,
					pCodecCtx->height,
					pFrameRGB->data,
					pFrameRGB->linesize
				);

				result = pFrameRGB->data[0];

				if (originAtBottom) {
					auto stride = pFrameRGB->linesize[0];
					std::vector<uint8_t> tmpBuffer(stride);
					uint8_t *org, *dst;
					for (int i = 0; i < pCodecCtx->height / 2; i++) {
						org = result + stride * i;
						dst = result + stride * (pCodecCtx->height - i - 1);
						memcpy(&tmpBuffer[0], org, stride);
						memcpy(org, dst, stride);
						memcpy(dst, &tmpBuffer[0], stride);
					}
				}
				done = true;
			}
		}

		// Free the packet that was allocated by av_read_frame
		av_packet_unref(&packet);
	}

  if (!done) {
    // We arrive here because av_read_frame failed. Let's investigate
    if (avreadReturnCode == AVERROR_EOF) {
      if (autoloop) {
        av_seek_frame(pFormatCtx, -1, 0, AVSEEK_FLAG_BACKWARD);
      }
      else {
        endOfVideo = true;
      }
    }
  }
	return result;
}

std::string media::ffmpegError(int errnum)
{
	char buf[128];
	av_strerror(errnum, buf, sizeof(buf));
	return std::string(buf);
}
