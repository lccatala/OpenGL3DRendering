#include "utils.h"
#include "textureCubeMap.h"
#include "log.h"
#include "image.h"

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

using PGUPV::TextureCubeMap;
using PGUPV::Image;

using std::string;

TextureCubeMap::TextureCubeMap(GLenum minfilter, GLenum magfilter,
	GLenum wrap_s, GLenum wrap_t)
	: Texture(GL_TEXTURE_CUBE_MAP, minfilter, magfilter, wrap_s, wrap_t) {
	loadedFaces = 0;
}

bool TextureCubeMap::loadImage(GLenum face, std::string filename, bool flipV,
	std::ostream * /* error_output */) {
	_ready = false;

	Image image(filename);

	if (flipV)
		image.flipV();

	glBindTexture(GL_TEXTURE_CUBE_MAP, _texId);
	/* Create and load textures to OpenGL */
	glTexImage2D(face, 0, image.getSuggestedGLInternalFormatType(), image.getWidth(), image.getHeight(), 0,
		image.getGLFormatType(), image.getGLPixelBaseType(), image.getPixels());
	switch (face) {
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		loadedFaces |= 1;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		loadedFaces |= 2;
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		loadedFaces |= 4;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		loadedFaces |= 8;
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		loadedFaces |= 16;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		loadedFaces |= 32;
		break;
	}
	if (loadedFaces == 63) {
		// All faces loaded?
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, _magfilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, _minfilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, _wrap_s);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, _wrap_t);
		_ready = true;
	}

	_name = PGUPV::getFilenameFromPath(filename);
	return _ready;
}

bool TextureCubeMap::loadDDS(std::string filename, std::ostream * /*error_output*/) {
	gli::texture Texture = gli::load(filename);
	if (Texture.empty())
		return false;

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

	glBindTexture(_texture_type, _texId);
	glTexParameteri(_texture_type, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(_texture_type, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(_texture_type, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(_texture_type, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(_texture_type, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(_texture_type, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(Texture.extent());

	glTexStorage2D(
		_texture_type, static_cast<GLint>(Texture.levels()), Format.Internal,
		Extent.x, Extent.y);

	for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
		for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				glm::tvec3<GLsizei> tExtent(Texture.extent(Level));
				GLenum glFace = static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face);

				if (gli::is_compressed(Texture.format()))
					glCompressedTexSubImage2D(
						glFace, static_cast<GLint>(Level),
						0, 0, tExtent.x, tExtent.y,
						Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
						Texture.data(Layer, Face, Level));
				else
					glTexSubImage2D(
						glFace, static_cast<GLint>(Level),
						0, 0, tExtent.x, tExtent.y,
						Format.External, Format.Type,
						Texture.data(Layer, Face, Level));
			}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, _magfilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, _minfilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, _wrap_s);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, _wrap_t);
	_ready = true;

	return _ready;
}

bool TextureCubeMap::loadImages(string filename, bool flipV,
	std::ostream *error_output) {
	CHECK_GL();

	string::size_type dot = filename.find_last_of('.');
	string::size_type slash = filename.find_first_of('/');
	if (slash == string::npos)
		slash = filename.find_first_of('/');

	if (dot == string::npos || dot < slash)
		ERRT("Debes dar la extensión de las texturas que forman el mapa cúbico "
			"(p.e. fichero.png)");

	string extension = filename.substr(dot);
	string basename = filename.substr(0, dot);

	loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, basename + "posx" + extension,
		flipV, error_output);
	loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, basename + "negx" + extension,
		flipV, error_output);
	loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, basename + "posy" + extension,
		flipV, error_output);
	loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, basename + "negy" + extension,
		flipV, error_output);
	loadImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, basename + "posz" + extension,
		flipV, error_output);
	return loadImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		basename + "negz" + extension, flipV, error_output);
}
