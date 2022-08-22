#include "textureGenerator.h"

using namespace PGUPV;
using namespace std;

Texture2D *TextureGenerator::makeChecker(const glm::vec4 &color1,
	const glm::vec4 &color2, unsigned int size, unsigned int numsquares) {

	std::vector<glm::vec4> pixels(size * size);

	glm::vec4 colors[2]{ color1, color2 };

	unsigned int pixelsPerSquare = size / numsquares;

	glm::vec4 *writeHere = &pixels[0];
	// Rows
	for (auto row = 0U; row < size; row++) {
		// What color is the first square?
		int color;
		if ((row / pixelsPerSquare) % 2 == 0)
			color = 0;
		else
			color = 1;

		uint offset = 0;
		// Draw a row of pixels
		while (offset + pixelsPerSquare < size) {
			// Draw a square of the current color 
			for (unsigned int i = 0; i < pixelsPerSquare; ++i) {
				*writeHere++ = colors[color];
			}
			offset += pixelsPerSquare;
			color = (color + 1) & 1;
		}

		// If there are pixels left, fill them up
		while (offset < size) {
			*writeHere++ = colors[color];
			offset++;
		}
	}

	auto result = new Texture2D();
	result->loadImageFromMemory(&pixels[0], size, size, GL_RGBA, GL_FLOAT, GL_RGBA8);
	result->setName("checkboard-texture");

	return result;
}

