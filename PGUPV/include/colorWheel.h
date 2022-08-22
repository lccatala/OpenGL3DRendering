#ifndef _COLOR_WHEEL
#define _COLOR_WHEEL 2013

#include <vector>
#include <glm/glm.hpp>
#include "common.h"

namespace PGUPV {

	class ColorWheel {
	public:
		static uchar *getColor(unsigned char index) { return palette + index * 4; }
		static const glm::vec4 &getColorVec4(unsigned char index);
	private:
		static uchar palette[256 * 4];
		static std::vector<glm::vec4> paletteVec4;
	};


};  // namespace PGUPV
#endif
