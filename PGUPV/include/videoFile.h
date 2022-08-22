#pragma once

#include <string>
#include "media.h"

namespace media {
	class VideoFile : public Media {
	public:
		VideoFile(const std::string &filepath);
        ~VideoFile();
		/**
		Salta al principio del vídeo
		*/
		void rewind();
	private:
		std::string filepath;
	};
};