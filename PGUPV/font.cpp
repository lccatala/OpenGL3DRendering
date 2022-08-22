#include <string>
#include <SDL_ttf.h>

#include "font.h"
#include "log.h"

using PGUPV::Font;

bool Font::ttfInitialized = false;
Font::FontCache Font::cache;


Font::Font(const std::string &filePath, int pointSize) {
	ttffont = TTF_OpenFont(filePath.c_str(), pointSize);
  if (ttffont == nullptr) {
    ERRT("No se ha podido encontrar la fuenete " + filePath);
  }
}

std::shared_ptr<Font> Font::loadFont(std::string filePath, int pointSize) {
	if (!ttfInitialized) {
		/* start SDL_ttf */
		if(TTF_Init() == -1)
			ERRT(std::string("TTF_Init: ") + TTF_GetError());
		ttfInitialized = true;
	}

	FontCache::iterator font = cache.find(filePath);
	if (font == cache.end()) {
		cache.insert(std::pair<std::string, SizeCache>(filePath, SizeCache()));
		font = cache.find(filePath);
	}
	SizeCache &sizes = font->second;
	SizeCache::iterator size = sizes.find(pointSize);
	if (size == sizes.end()) {
		std::shared_ptr<Font> theFont = std::shared_ptr<Font>(new Font(filePath, pointSize));
		sizes.insert(std::pair<int, std::shared_ptr<Font>>(pointSize, theFont));
		return theFont;
	} else 
		return size->second;
}

std::shared_ptr<Font> Font::getDefaultFont() {
	return loadFont(DEFAULT_FONT, DEFAULT_TEXT_SIZE);
}
