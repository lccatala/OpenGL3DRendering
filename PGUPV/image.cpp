
#include <sstream>
#include <iomanip>
#include <memory.h>


#include "image.h"
#include "log.h"
#include "utils.h"

#define GLM_ENABLE_EXPERIMENTAL

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4458 4100)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <gli/gli.hpp>
#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

using PGUPV::Image;

bool Image::_freeImageInitialized = false;

// Carga la imagen desde el fichero indicado
Image::Image(std::string filename) : _data(nullptr), _filename(filename),
freeimageImage(nullptr), freeimageMultiImage(nullptr) {
	initLib();
	if (!load(filename))
		ERRT(std::string("No se ha podido cargar la imagen ") + filename);
}

Image::Image(Image &&other) : _width(other._width), _height(other._height),
_data(other._data), _bpp(other._bpp), _nfaces(other._nfaces),
_nAnimationFrames(other._nAnimationFrames), _stride(other._stride),
freeimageImage(other.freeimageImage), freeimageMultiImage(other.freeimageMultiImage)
{
	other.freeimageImage = nullptr;
	other.freeimageMultiImage = nullptr;
	other._data = nullptr;
}

// Crea una imagen con el tamaño indicado. Opcionalmente, copia la información apuntada por data
Image::Image(uint width, uint height, uint bpp, void *data) :
	_width(width), _height(height), _bpp(bpp), _nfaces(1), _nAnimationFrames(1), _stride(width * bpp / 8),
	freeimageImage(nullptr), freeimageMultiImage(nullptr)
{
	if (bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32) {
		ERRT("Sólo se aceptan imágenes en escala de grises de 8 o 16 BPP o RGB de 24 BPP o RGBA de 32 BPP");
	}
	initLib();
	_data = new uchar*[1];
	_data[0] = new uchar[_stride * height];
	if (data) {
		memcpy(_data[0], data, _stride * height);
	}
}

Image::~Image() {
	releaseMemory();
}

GLenum Image::getGLPixelBaseType() const {
	if (freeimageImage != nullptr || freeimageMultiImage != nullptr) {
		FREE_IMAGE_TYPE type;
		if (freeimageImage != nullptr)
			type = FreeImage_GetImageType(freeimageImage);
		else
			type = FreeImage_GetImageType(lockedPages[0]);

		switch (type) {
		case FIT_BITMAP:
			if (_bpp == 1 || _bpp == 4)
				ERRT("Tipo de imagen desconocido");
			else
				return GL_UNSIGNED_BYTE;
		case FIT_UINT16:
			return GL_UNSIGNED_SHORT;
		case FIT_INT16:
		case FIT_RGB16:
		case FIT_RGBA16:
			return GL_SHORT;
		case FIT_UINT32:
			return GL_UNSIGNED_INT;
		case FIT_INT32:
			return GL_INT;
		case FIT_FLOAT:
		case FIT_RGBF:
		case FIT_RGBAF:
			return GL_FLOAT;
		case FIT_DOUBLE:
		case FIT_COMPLEX:
			return GL_DOUBLE;
		default:
			ERRT("Tipo de imagen desconocido");
		}
	}
	else {
		switch (_bpp) {
		case 8:
		case 16:
		case 24:
		case 32:
			return GL_UNSIGNED_BYTE;
			break;
		}
	}
	ERRT("Tipo de imagen desconocido");
}

GLenum Image::getSuggestedGLInternalFormatType() const
{
	if (freeimageImage != nullptr || freeimageMultiImage != nullptr) {
		FREE_IMAGE_TYPE type;
		if (freeimageImage != nullptr)
			type = FreeImage_GetImageType(freeimageImage);
		else {
			// El primer frame siempre está cargado
			type = FreeImage_GetImageType(lockedPages[0]);
		}

		switch (type) {
		case FIT_BITMAP:
			switch (_bpp) {
			case 8:
				return GL_R8;
			case 16:
				return GL_RG8;
			case 24:
				return GL_RGB8;
			case 32:
				return GL_RGBA8;
			default:
				ERRT("Tipo de imagen desconocido");

			}
		case FIT_UINT16:
			return GL_R16UI;
		case FIT_INT16:
			return GL_R16I;
		case FIT_RGB16:
			return GL_RGB16UI;
		case FIT_RGBA16:
			return GL_RGBA16UI;
		case FIT_UINT32:
			return GL_R32UI;
		case FIT_INT32:
			return GL_R32I;
		case FIT_FLOAT:
			return GL_R32F;
		case FIT_RGBF:
			return GL_RGB32F;
		case FIT_RGBAF:
			return GL_RGBA32F;
		case FIT_DOUBLE:
		case FIT_COMPLEX:
		default:
			ERRT("Tipo de imagen desconocido");
		}
	}
	else if (_data[0] != nullptr) {
		switch (_bpp) {
		case 8:
			return GL_R8;
			break;
		case 16:
			return GL_RG8;
		case 24:
			return GL_RGB8;
			break;
		case 32:
			return GL_RGBA8;
			break;
		default:
			ERRT("Tipo de texel no soportado");
		}
	}
	else ERRT("Tipo de imagen desconocido");
}

GLenum Image::getGLFormatType() const {
	if (freeimageImage != nullptr || freeimageMultiImage != nullptr) {
		FREE_IMAGE_TYPE type;
		if (freeimageImage != nullptr)
			type = FreeImage_GetImageType(freeimageImage);
		else {
			// El primer frame siempre está cargado
			type = FreeImage_GetImageType(lockedPages[0]);
		}
		switch (type) {
		case FIT_BITMAP:
			switch (_bpp) {
			case 8:
				return GL_RED;
			case 16:
				return GL_RG;
			case 24:
				return GL_RGB;
			case 32:
				return GL_RGBA;
			default:
				ERRT("Tipo de imagen desconocido");
			}
		case FIT_UINT16:
		case FIT_INT16:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_FLOAT:
			return GL_RED;
		case FIT_RGB16:
		case FIT_RGBF:
			return GL_RGB;
		case FIT_RGBA16:
		case FIT_RGBAF:
			return GL_RGBA;
		case FIT_DOUBLE:
		case FIT_COMPLEX:
		default:
			ERRT("Tipo de imagen desconocido");
		}
	}
	else if (_data[0] != nullptr) {
		switch (_bpp) {
		case 8:
			return GL_RED;
			break;
		case 16:
			return GL_RG;
		case 24:
			return GL_RGB;
			break;
		case 32:
			return GL_RGBA;
			break;
		default:
			ERRT("Tipo de texel no soportado");
		}
	}
	else
		ERRT("Tipo de imagen desconocido");
}

void Image::releaseMemory() {

	if (freeimageImage != nullptr) {
		FreeImage_Unload(freeimageImage);
		delete[]_data;
	}
	else if (freeimageMultiImage != nullptr) {
		for (uint i = 0; i < this->_nAnimationFrames; i++) {
			FreeImage_UnlockPage(freeimageMultiImage, lockedPages[i], FALSE);
		}
		FreeImage_CloseMultiBitmap(freeimageMultiImage);
		lockedPages.clear();
		delete[]_data;
	}
	else {
		if (_data != nullptr) {
			uint nLayers = MAX(_nfaces, _nAnimationFrames);
			for (uint i = 0; i < nLayers; i++) {
				delete[] _data[i];
			}
			delete[] _data;
			_data = nullptr;
		}
	}
}

static std::string FreeImageErrorMsg;

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	if (fif != FIF_UNKNOWN) {
		FreeImageErrorMsg = std::string("Formato: ") + FreeImage_GetFormatFromFIF(fif);
	}
	else
		FreeImageErrorMsg = "";
	FreeImageErrorMsg += message;
}

void Image::initLib() {
	if (!_freeImageInitialized) {
#ifndef _WIN32
		FreeImage_Initialise();
#endif
		FreeImage_SetOutputMessage(FreeImageErrorHandler);
		_freeImageInitialized = true;
}
}

bool Image::loadDDS(const std::string &/*filename*/) {
	ERRT("No implementado (habla con Paco)");
	// Si hace falta, usar una TextureCubeMap para cargar el dds y luego pedir las caras a OpenGL
}

const std::string Image::getLibraryInfo() {
	static const std::string gliVersion("GLI: " STRINGIFY(GLI_VERSION_MAJOR) "." STRINGIFY(GLI_VERSION_MINOR) "." STRINGIFY(GLI_VERSION_PATCH) "\n");
	std::string freeImageVersion = std::string("FreeImage: ") + FreeImage_GetVersion();
	return gliVersion + freeImageVersion;
}

bool Image::loadFreeImage(::FIBITMAP *image, const uint frame) {
	_width = FreeImage_GetWidth(image);
	_height = FreeImage_GetHeight(image);
	_nfaces = 1;
	_bpp = FreeImage_GetBPP(image);
	_stride = FreeImage_GetPitch(image);

	if (_nfaces != 1 && _nAnimationFrames > 1)
		ERRT("No se soportan entornos cúbicos animados");
	else if (_nfaces == 1) {
		_data[frame] = FreeImage_GetBits(image);
#if FREEIMAGE_COLORORDER==FREEIMAGE_COLORORDER_BGR
		if (_bpp == 24 || _bpp == 32) swapRB(frame);
#endif
	}
	return true;
}

bool Image::loadSimple(const std::string &filename, const FREE_IMAGE_FORMAT fileType) {
	if (fileType == FIF_GIF)
		freeimageImage = FreeImage_Load(fileType, filename.c_str(), GIF_PLAYBACK);
	else
		freeimageImage = FreeImage_Load(fileType, filename.c_str());
	if (freeimageImage == nullptr) {
		ERR("No se ha podido cargar la imagen " + filename + "Error: " + FreeImageErrorMsg);
		return false;
	}
	_data = new uchar *[1];
	_nAnimationFrames = 1;

	return loadFreeImage(freeimageImage);
}

bool Image::loadMulti(const std::string &filename, const FREE_IMAGE_FORMAT fileType) {
	if (fileType == FIF_GIF)
		freeimageMultiImage = FreeImage_OpenMultiBitmap(fileType, filename.c_str(), false, true, true, GIF_PLAYBACK);
	else
		freeimageMultiImage = FreeImage_OpenMultiBitmap(fileType, filename.c_str(), false, true, true);

	if (freeimageMultiImage == nullptr) {
		ERRT("No se ha podido cargar la imagen " + filename + "Error: " + FreeImageErrorMsg);
	}

	auto pageCount = FreeImage_GetPageCount(freeimageMultiImage);
	if (pageCount == 1) {
		FreeImage_CloseMultiBitmap(freeimageMultiImage);
		return loadSimple(filename, fileType);
	}

	_nAnimationFrames = pageCount;
	_data = new uchar *[pageCount];
	lockedPages.resize(pageCount, nullptr);
	for (int i = 0; i < pageCount; i++) {
		_data[i] = nullptr;
	}


	FIBITMAP *dib = FreeImage_LockPage(freeimageMultiImage, 0);
	if (dib) {
		_width = FreeImage_GetWidth(dib);
		_height = FreeImage_GetHeight(dib);
		_nfaces = 1;
		_bpp = FreeImage_GetBPP(dib);
		_stride = FreeImage_GetPitch(dib);
		FreeImage_UnlockPage(freeimageMultiImage, dib, false);
	}
	else
		ERRT("No se ha podido leer el primer frame de " + filename);

	// Siempre cargamos el primer frame
	loadFrameFromMulti(0);
	return true;
}

// Carga la imagen en el fichero indicado. La imagen que contenía este objeto se destruye (incluyendo el
// puntero que devolvería getData
bool Image::load(std::string filename) {
	releaseMemory();

	if (!fileExists(filename))
		return false;

	auto fileType = FreeImage_GetFileType(filename.c_str(), 0);
	if (fileType == FIF_UNKNOWN) return false;

	if (fileType == FIF_UNKNOWN)
		fileType = FreeImage_GetFIFFromFilename(filename.c_str());

	//if (fileType == FIF_DDS)
	  //return loadDDS(filename);

	if (fileType == FIF_UNKNOWN)
		return false;

	if (!FreeImage_FIFSupportsReading(fileType))
		return false;

	if (fileType == FIF_GIF || fileType == FIF_ICO || fileType == FIF_TIFF)
		return loadMulti(filename, fileType);
	else
		return loadSimple(filename, fileType);
}

void flipVImage(uchar *data, uint stride, uint height) {
	uchar *tmp = new uchar[stride];
	for (uint i = 0; i < height / 2; i++) {
		// Swap scanlines i and _height-1-i
		memcpy(tmp, data + stride * i, stride);
		memcpy(data + stride * i, data + (height - i - 1) * stride, stride);
		memcpy(data + (height - i - 1) * stride, tmp, stride);
	}
	delete[] tmp;
}

void Image::flipV() {

	if (!_data || _width == 0 || _height == 0 || _bpp == 0) {
		ERRT("No hay imagen que reflejar verticalmente");
	}

	for (uint i = 0; i < _nfaces; i++) {
		flipVImage(_data[i], _stride, _height);
	}
}


uint maxDiffPixelRow8bpp(uchar *first, uchar *second, uint width) {
	int maxDifference = 0;
	for (unsigned int i = 0; i < width; i++) {
		maxDifference = std::max(maxDifference, std::abs(*first - *second));
		first++;
		second++;
	}
	return maxDifference;
}

uint maxDiffPixelRow(uchar *first, uchar *second, uint width, uint bppFirst, uint bppSecond) {
	if (bppFirst == 8 && bppSecond == 8) return maxDiffPixelRow8bpp(first, second, width);

	assert((bppFirst == 24 || bppFirst == 32) && (bppSecond == 24 || bppSecond == 32));
	int maxDiff = 0;
	int extraByteFirst = 0, extraByteSecond = 0;
	if (bppFirst == 32) extraByteFirst = 1;
	if (bppSecond == 32) extraByteSecond = 1;

	int alpha1 = 255, alpha2 = 255;
	for (uint i = 0; i < width; i++) {
		if (bppFirst == 32) alpha1 = first[3];
		if (bppSecond == 32) alpha2 = second[3];
		for (uint j = 0; j < 3; j++) {
			int d = abs(((int)*first)*alpha1 - ((int)*second)*alpha2) / 255;
			maxDiff = std::max(maxDiff, d);
			first++;
			second++;
		}
		first += extraByteFirst;
		second += extraByteSecond;
	}
	return maxDiff;
}

bool Image::equals(Image &other, uint maxDifference) {
	if (&other == this) return true;
	if (_height != other.getHeight() ||
		_width != other.getWidth() ||
		_nfaces != other.getNumFaces())
		return false;

	if ((_bpp != 8 && _bpp != 24 && _bpp != 32) ||
		(other._bpp != 8 && other._bpp != 24 && other._bpp != 32))
		ERRT("Sólo se pueden comparar imágenes de 8, 24 o 32 bpp");

	if ((_bpp == 8 || other._bpp == 8) && _bpp != other._bpp) {
		ERRT("Las imágenes de 8 bpp sólo se pueden comparar con imágenes de 8 bpp");
	}


	for (uint i = 0; i < _nfaces; i++) {
		for (uint y = 0; y < _height; y++) {
			uint d = maxDiffPixelRow(
				static_cast<uchar *>(getPixels(0, y, i)),
				static_cast<uchar *>(other.getPixels(0, y, i)),
				_width, _bpp, other.getBPP());
			if (d > maxDifference) {
				INFO("Diferencia máxima encontrada hasta ahora: " + std::to_string(d));
				return false;
			}
		}
	}
	return true;
}

void Image::swapRB(uint frame)  const {
	if (_bpp != 24 && _bpp != 32)
		ERRT("No se puede intercambiar los canales de color de esta imagen");
	// swap R and B channels
	for (uint j = 0; j < _height; j++) {
		unsigned char *first = _data[frame] + _stride * j;
		for (uint i = 0; i < _width; i++) {
			std::swap(*first, *(first + 2));
			first += _bpp / 8;
		}
	}
}

bool Image::save(const std::string &filename, uint frame) {
	if (frame >= getNumFaces() && frame >= getAnimationFrames()) {
		ERRT("No existe ese frame o cara");
	}
#if FREEIMAGE_COLORORDER==FREEIMAGE_COLORORDER_BGR
	if (_bpp > 8) {
		swapRB(frame);
	}
#endif
	FIBITMAP* image = FreeImage_ConvertFromRawBits(_data[frame], _width, _height, _stride, _bpp, 0x0000FF, 0x00FF00, 0xFF0000, false);

	auto type = FreeImage_GetFIFFromFilename(filename.c_str());
	if (type == FIF_UNKNOWN) {
		ERRT("Extensión desconocida: " + filename);
	}
	FreeImage_Save(type, image, filename.c_str(), 0);

#if FREEIMAGE_COLORORDER==FREEIMAGE_COLORORDER_BGR
	if (_bpp > 8) {
		swapRB(frame);
	}
#endif
	FreeImage_Unload(image);
	return true;
}

Image *Image::extractFrame(uint frame) const {

	if (frame >= getAnimationFrames() && frame >= getNumFaces())
		ERRT("No existe el frame pedido");

	if (_data[frame] == nullptr) {
		loadFrameFromMulti(frame);
	}
	return new Image(_width, _height, _bpp, _data[frame]);
}

Image *Image::difference(Image &other, bool ignoreAlpha) {
	if (getWidth() != other.getWidth() || getHeight() != other.getHeight())
		ERRT("Las imágenes deben tener el mismo tamaño para calcular su diferencia");
	if (getNumFaces() != 1 || other.getNumFaces() != 1)
		ERRT("Sólo se puede calcular la diferencia de una imagen con una capa");
	if ((getBPP() != 24 && getBPP() != 32) || (other.getBPP() != 24 && other.getBPP() != 32))
		ERRT("Sólo se puede calcular la diferencia de imágenes de 24 y 32 bpp");

	uint outputBPP = MAX(getBPP(), other.getBPP());
	if (ignoreAlpha && outputBPP == 32) outputBPP = 24;
	Image *diff = new Image(getWidth(), getHeight(), outputBPP);
	uchar *output, *in1, *in2;
	const uint minBPP = MIN(getBPP(), other.getBPP());
	const uint numChannels = minBPP == 32 ? 4 : 3;
	for (uint y = 0; y < getHeight(); y++) {
		output = static_cast<uchar *>(diff->getPixels(0, y));
		in1 = static_cast<uchar *>(this->getPixels(0, y));
		in2 = static_cast<uchar *>(other.getPixels(0, y));

		for (uint x = 0; x < getWidth(); x++) {
			for (uint o = 0; o < numChannels; o++) {
				*output = static_cast<uint8_t>(abs(*in1 - *in2));
				output++;
				in1++;
				in2++;
			}
			if (numChannels == 3) {
				if (this->getBPP() == 32) in1++;
				if (other.getBPP() == 32) in2++;
				if (outputBPP == 32) {
					*output = 255;
					output++;
				}
			}
		}
	}

	return diff;
}

void Image::loadFrameFromMulti(const unsigned int frame) const {
	assert(freeimageMultiImage != nullptr);
	assert(frame < _nAnimationFrames);
	FIBITMAP *dib = FreeImage_LockPage(freeimageMultiImage, frame);
	if (dib) {
		_data[frame] = FreeImage_GetBits(dib);
#if FREEIMAGE_COLORORDER==FREEIMAGE_COLORORDER_BGR
		swapRB(frame);
#endif
		if (lockedPages[frame] != nullptr) {
			FreeImage_UnlockPage(freeimageMultiImage, lockedPages[frame], false);
		}
		lockedPages[frame] = dib;
	}
}

void *Image::getPixels(uint x, uint y, uint layer) const {

	ulong offset = y * _stride + x * _bpp / 8;

	// ¿Es una animación?
	if (freeimageMultiImage != nullptr) {
		if (layer >= _nAnimationFrames)
			ERRT("La animación no tiene el frame pedido");
		else {
			if (_data[layer] == nullptr) {
				loadFrameFromMulti(layer);
			}
		}
	}
	else if (layer >= _nfaces) // ¿Es un mapa cúbico?
		ERRT("El cubo no tiene esa cara");

	return static_cast<unsigned char *>(_data[layer]) + offset;
}


Image Image::convert8BPPGrayTo24BPPGray(const Image &src) {
	Image dst(src._width, src._height, 24);

	uint8_t *dst_pixels = static_cast<uint8_t *>(dst.getPixels());
	uint8_t *src_pixels = static_cast<uint8_t *>(src.getPixels());
	for (unsigned long y = 0; y < src._height; y++) {
		uint8_t *dst_row = dst_pixels + dst.getStride() * y;
		uint8_t *src_row = src_pixels + src.getStride() * y;
		for (unsigned long x = 0; x < src._width; x++) {
			*dst_row++ = *src_row;
			*dst_row++ = *src_row;
			*dst_row++ = *src_row;
			src_row++;
		}
	}

	return dst;
}

bool Image::save(const std::string & filename, uint32_t width, uint32_t height, uint32_t bpp, uint8_t * bytes)
{
	Image tmp(width, height, bpp, bytes);
	return tmp.save(filename);
}


bool Image::saveHDR(const std::string& filename, uint32_t width, uint32_t height, uint32_t bpp, const float* bytes)
{
	initLib();
	auto* image = FreeImage_AllocateT(FIT_RGBF, width, height, 96);
	float* to = reinterpret_cast<float *>(FreeImage_GetBits(image));
	
	if (bpp == 96) {
		memcpy(to, bytes, static_cast<size_t>(width) * height * 3 * sizeof(float));
	}
	else if (bpp == 32) {
		const float* from = bytes;
		for (size_t i = 0; i < static_cast<size_t>(width) * height; i++) {
			*to++ = *from;
			*to++ = *from;
			*to++ = *from;
			from++;
		}
	}
	else {
		ERRT("Unsupported BPP for saving HDR images");
	}

	if (!FreeImage_Save(FIF_HDR, image, filename.c_str(), 0))
		return false;

	FreeImage_Unload(image);
	return true;
}