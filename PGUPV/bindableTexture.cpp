//
//  bindableTexture.cpp
//  PGUPV
//
//  Created by Paco Abad on 13/11/13.
//  Copyright (c) 2013 Paco Abad. All rights reserved.
//

#include "log.h"
#include "bindableTexture.h"
#include "utils.h"

using PGUPV::BindableTexture;
using std::string;

BindableTexture::BindableTexture(GLenum texture_type)
	: _ready(false), _texture_type(texture_type), _textureUnitBound(-1) {
	glGenTextures(1, &_texId);
#ifdef _DEBUG
#define TARGET(b)                                                              \
  case b:                                                                      \
    type = #b;                                                                 \
    break
	std::string type;
	switch (texture_type) {
		TARGET(GL_TEXTURE_1D);
		TARGET(GL_TEXTURE_2D);
		TARGET(GL_TEXTURE_3D);
		TARGET(GL_TEXTURE_1D_ARRAY);
		TARGET(GL_TEXTURE_2D_ARRAY);
		TARGET(GL_TEXTURE_RECTANGLE);
		TARGET(GL_TEXTURE_BUFFER);
		TARGET(GL_TEXTURE_CUBE_MAP);
		TARGET(GL_TEXTURE_CUBE_MAP_ARRAY);
		TARGET(GL_TEXTURE_2D_MULTISAMPLE);
		TARGET(GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
	default:
		type = "UNKNOWN";
		break;
	}
#undef TARGET
	INFO("Textura de tipo " + type + " creada con id " + std::to_string(_texId));
#endif
}

BindableTexture::~BindableTexture() {
    glDeleteTextures(1, &_texId);
    INFO("Textura con id " + std::to_string(_texId) + " destruída");
}

void BindableTexture::bind(GLenum textureUnit) {
	if (!_ready)
		ERRT("No se puede vincular una textura no preparada. "
		"Llama antes a loadImage y comprueba que haya funcionado");

	glActiveTexture(textureUnit);
	glBindTexture(_texture_type, _texId);
	_textureUnitBound = textureUnit;
	FRAME("Textura " + std::to_string(_texId) +
		" conectada en unidad de textura " +
		std::to_string(textureUnit - GL_TEXTURE0));
	CHECK_GL();
}

void BindableTexture::unbind() {
	if (!_ready || _textureUnitBound == -1)
		ERRT("No se puede desvincular una textura no vinculada");

	glActiveTexture(_textureUnitBound);
	glBindTexture(_texture_type, 0);
	_textureUnitBound = -1;
	FRAME("Textura " + std::to_string(_texId) +
		" desconectada de la unidad de textura");
}

void BindableTexture::clear(int level, GLenum format, GLenum type, const void *data) {
	glClearTexImage(getId(), level, format, type, data);
}
