#ifndef _FONT_H
#define _FONT_H 2014

#include <memory>
#include <map>

#include <string>    // for string

#define DEFAULT_FONT "../recursos/fuentes/FreeSans.ttf"
#define DEFAULT_TEXT_SIZE 12

struct _TTF_Font;
typedef _TTF_Font TTF_Font;

namespace PGUPV {
class Font {
public:
	static std::shared_ptr<Font> loadFont(std::string filePath, int pointSize);
	static std::shared_ptr<Font> getDefaultFont();
	TTF_Font *getTTFFont() {return ttffont;};
private:
	Font(const std::string &filePath, int pointSize);
	typedef std::map<int, std::shared_ptr<Font>> SizeCache;
	typedef std::map<std::string, SizeCache> FontCache;
	// Fonts are never released, so this is a perfect example of a memory leak
	static FontCache cache;
	static bool ttfInitialized;
	TTF_Font *ttffont;
};
};
#endif
