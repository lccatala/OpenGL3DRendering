#include <algorithm>

#include "pbrMaterial.h"
#include "indexedBindingPoint.h"
#include "uboPBRMaterial.h"
#include "bindableTexture.h"


using PGUPV::PBRMaterial;
using PGUPV::BindableTexture;

PBRMaterial::PBRMaterial() : BaseMaterial("Default PBR Material") {
  uboPBRMaterial = UBOPBRMaterial::build();
}

PBRMaterial::PBRMaterial(const std::string &name) : BaseMaterial(name) {
  uboPBRMaterial = UBOPBRMaterial::build();
}

unsigned int PBRMaterial::getTextureCount(TextureType type) {
	auto texcount = uboPBRMaterial->getMaterial().textureCount;
	switch (type) {
	case BASECOLOR_TUNIT:
		return texcount & 0x7;
	case NORMAL_TUNIT:
		return (texcount >> 3) & 0x7;
	case EMISSION_TUNIT:
		return (texcount >> 6) & 0x7;
	case METALNESS_TUNIT:
		return (texcount >> 9) & 0x7;
	case ROUGHNESS_TUNIT:
		return (texcount >> 12) & 0x7;
	case AMBIENTOCLUSSION_TUNIT:
		return (texcount >> 15) & 0x7;
	default:
		ERRT("Tipo de textura desconocido");
	}
}

void PBRMaterial::setTextureCount(TextureType type, unsigned int count) {
	auto texcount = uboPBRMaterial->getMaterial().textureCount;
	switch (type) {
	case BASECOLOR_TUNIT:
		texcount = (texcount & ~0x7) + count;
		break;
	case NORMAL_TUNIT:
		texcount = (texcount & ~(0x7 << 3)) + (count << 3);
		break;
	case EMISSION_TUNIT:
		texcount = (texcount & ~(0x7 << 6)) + (count << 6);
		break;
	case METALNESS_TUNIT:
		texcount = (texcount & ~(0x7 << 9)) + (count << 9);
		break;
	case ROUGHNESS_TUNIT:
		texcount = (texcount & ~(0x7 << 12)) + (count << 12);
		break;
	case AMBIENTOCLUSSION_TUNIT:
		texcount = (texcount & ~(0x7 << 15)) + (count << 15);
		break;
	}
	auto mat = uboPBRMaterial->getMaterial();
	mat.textureCount = texcount;
	uboPBRMaterial->setMaterial(mat);
}

unsigned int PBRMaterial::getTextureCounters() const {
  return uboPBRMaterial->getMaterial().textureCount;
}

void PBRMaterial::setTexture(unsigned int tex_unit, std::shared_ptr<PGUPV::BindableTexture> tex) {
	texs[tex_unit] = tex;
	// Update the texture counters
	if (tex_unit < NORMAL_TUNIT) {
		setTextureCount(BASECOLOR_TUNIT, std::max(getTextureCount(BASECOLOR_TUNIT), tex_unit - BASECOLOR_TUNIT + 1));
	}
	else if (tex_unit < EMISSION_TUNIT) {
		setTextureCount(NORMAL_TUNIT, std::max(getTextureCount(NORMAL_TUNIT), tex_unit - NORMAL_TUNIT + 1));
	}
	else if (tex_unit < METALNESS_TUNIT) {
		// NORMAL
		setTextureCount(EMISSION_TUNIT, std::max(getTextureCount(EMISSION_TUNIT), tex_unit - EMISSION_TUNIT + 1));
	}
	else if (tex_unit < ROUGHNESS_TUNIT) {
		// HEIGHT
		setTextureCount(METALNESS_TUNIT, std::max(getTextureCount(METALNESS_TUNIT), tex_unit - METALNESS_TUNIT + 1));
	}
	else if (tex_unit < AMBIENTOCLUSSION_TUNIT) {
		// OPACITY
		setTextureCount(ROUGHNESS_TUNIT, std::max(getTextureCount(ROUGHNESS_TUNIT), tex_unit - ROUGHNESS_TUNIT + 1));
	}
	else {
		// AMBIENT
		setTextureCount(AMBIENTOCLUSSION_TUNIT, std::max(getTextureCount(AMBIENTOCLUSSION_TUNIT), tex_unit - AMBIENTOCLUSSION_TUNIT + 1));
	}
}

std::shared_ptr<BindableTexture> PBRMaterial::getTexture(unsigned int tex_unit)
{
	return texs[tex_unit];
}

void PBRMaterial::setBaseColorTexture(std::shared_ptr<BindableTexture> tex, unsigned int index)
{
	setTexture(BASECOLOR_TUNIT + index, tex);
}

void PBRMaterial::setEmissionTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(EMISSION_TUNIT + index, tex);
}

void PBRMaterial::setNormalMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(NORMAL_TUNIT + index, tex);
}

void PBRMaterial::setMetalnessTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(METALNESS_TUNIT+ index, tex);
}

void PBRMaterial::setRoughnessTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(ROUGHNESS_TUNIT+ index, tex);
}

void PBRMaterial::setAmbientOcTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(AMBIENTOCLUSSION_TUNIT + index, tex);
}

void PBRMaterial::use() {
	gl_uniform_buffer.bindBufferBase(uboPBRMaterial, UBO_PBR_MATERIALS_BINDING_INDEX);
	for (auto t : texs) {
		t.second->bind(GL_TEXTURE0 + t.first);
	}
}


void PBRMaterial::unuse() {
	for (auto t : texs) {
		t.second->unbind();
	}
}