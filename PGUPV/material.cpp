#include <algorithm>

#include "material.h"
#include "indexedBindingPoint.h"
#include "uboMaterial.h"
#include "bindableTexture.h"


using PGUPV::Material;
using PGUPV::BindableTexture;

Material::Material() : BaseMaterial("Default material") {
  ubomaterial = UBOMaterial::build();
}

Material::Material(const std::string &name) : BaseMaterial(name) {
  ubomaterial = UBOMaterial::build();
}

Material::Material(const std::string &name, const glm::vec4 &amb, const glm::vec4 &dif,
  const glm::vec4 &spec, const glm::vec4 &emi, float shininess) : BaseMaterial(name) {
  ubomaterial = UBOMaterial::build(amb, dif, spec, emi, shininess);
}


unsigned int Material::getTextureCount(TextureType type) {
	auto texcount = ubomaterial->getMaterial().textureCount;
	switch (type) {
	case DIFFUSE_TUNIT:
		return texcount & 0x7;
	case SPECULAR_TUNIT:
		return (texcount >> 3) & 0x7;
	case NORMALMAP_TUNIT:
		return (texcount >> 6) & 0x7;
	case HEIGHTMAP_TUNIT:
		return (texcount >> 9) & 0x7;
	case OPACITYMAP_TUNIT:
		return (texcount >> 12) & 0x7;
	case AMBIENT_TUNIT:
		return (texcount >> 15) & 0x7;
	default:
		ERRT("Tipo de textura desconocido");
	}
}

void Material::setTextureCount(TextureType type, unsigned int count) {
	auto texcount = ubomaterial->getMaterial().textureCount;
	switch (type) {
	case DIFFUSE_TUNIT:
		texcount = (texcount & ~0x7) + count;
		break;
	case SPECULAR_TUNIT:
		texcount = (texcount & ~(0x7 << 3)) + (count << 3);
		break;
	case NORMALMAP_TUNIT:
		texcount = (texcount & ~(0x7 << 6)) + (count << 6);
		break;
	case HEIGHTMAP_TUNIT:
		texcount = (texcount & ~(0x7 << 9)) + (count << 9);
		break;
	case OPACITYMAP_TUNIT:
		texcount = (texcount & ~(0x7 << 12)) + (count << 12);
		break;
	case AMBIENT_TUNIT:
		texcount = (texcount & ~(0x7 << 15)) + (count << 15);
		break;
	}
	auto mat = ubomaterial->getMaterial();
	mat.textureCount = texcount;
	ubomaterial->setMaterial(mat);
}

unsigned int Material::getTextureCounters() const {
  return ubomaterial->getMaterial().textureCount;
}

void Material::setTexture(unsigned int tex_unit, std::shared_ptr<PGUPV::BindableTexture> tex) {
	texs[tex_unit] = tex;
	// Update the texture counters
	if (tex_unit < SPECULAR_TUNIT) {
		// DIFFUSE
		setTextureCount(DIFFUSE_TUNIT, std::max(getTextureCount(DIFFUSE_TUNIT), tex_unit - DIFFUSE_TUNIT + 1));
	}
	else if (tex_unit < NORMALMAP_TUNIT) {
		// SPECULAR
		setTextureCount(SPECULAR_TUNIT, std::max(getTextureCount(SPECULAR_TUNIT), tex_unit - SPECULAR_TUNIT + 1));
	}
	else if (tex_unit < HEIGHTMAP_TUNIT) {
		// NORMAL
		setTextureCount(NORMALMAP_TUNIT, std::max(getTextureCount(NORMALMAP_TUNIT), tex_unit - NORMALMAP_TUNIT + 1));
	}
	else if (tex_unit < OPACITYMAP_TUNIT) {
		// HEIGHT
		setTextureCount(HEIGHTMAP_TUNIT, std::max(getTextureCount(HEIGHTMAP_TUNIT), tex_unit - HEIGHTMAP_TUNIT + 1));
	}
	else if (tex_unit < AMBIENT_TUNIT) {
		// OPACITY
		setTextureCount(OPACITYMAP_TUNIT, std::max(getTextureCount(OPACITYMAP_TUNIT), tex_unit - OPACITYMAP_TUNIT + 1));
	}
	else {
		// AMBIENT
		setTextureCount(AMBIENT_TUNIT, std::max(getTextureCount(AMBIENT_TUNIT), tex_unit - AMBIENT_TUNIT + 1));
	}
}

std::shared_ptr<BindableTexture> Material::getTexture(unsigned int tex_unit)
{
	return texs[tex_unit];
}

const glm::vec4 & Material::getAmbient() const
{
	return ubomaterial->getMaterial().ambient;
}

const glm::vec4 &Material::getDiffuse() const
{
	return ubomaterial->getMaterial().diffuse;
}

const glm::vec4 &Material::getSpecular() const {
	return ubomaterial->getMaterial().specular;

}
const glm::vec4 &Material::getEmissive() const {
	return ubomaterial->getMaterial().emissive;

}
float Material::getShininess() const {
	return ubomaterial->getMaterial().shininess;
}

void Material::setAmbient(const glm::vec4 &c) {
	auto mat = ubomaterial->getMaterial();
	mat.ambient = c;
	ubomaterial->setMaterial(mat);
}

void Material::setDiffuse(const glm::vec4 &c) {
	auto mat = ubomaterial->getMaterial();
	mat.diffuse= c;
	ubomaterial->setMaterial(mat);
}

void Material::setSpecular(const glm::vec4 &c) {
	auto mat = ubomaterial->getMaterial();
	mat.specular = c;
	ubomaterial->setMaterial(mat);
}

void Material::setEmissive(const glm::vec4 &c) {
	auto mat = ubomaterial->getMaterial();
	mat.emissive = c;
	ubomaterial->setMaterial(mat);
}

void Material::setShininess(float s) {
	auto mat = ubomaterial->getMaterial();
	mat.shininess = s;
	ubomaterial->setMaterial(mat);
}


void Material::setDiffuseTexture(std::shared_ptr<BindableTexture> tex, unsigned int index)
{
	setTexture(DIFFUSE_TUNIT + index, tex);
}

void Material::setSpecularTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(SPECULAR_TUNIT + index, tex);
}

void Material::setNormalMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(NORMALMAP_TUNIT + index, tex);
}

void Material::setHeightMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(HEIGHTMAP_TUNIT + index, tex);
}

void Material::setOpacitytMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(OPACITYMAP_TUNIT + index, tex);
}

void Material::setAmbientMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index) {
	setTexture(AMBIENT_TUNIT + index, tex);
}

void Material::use() {
	gl_uniform_buffer.bindBufferBase(ubomaterial, UBO_MATERIALS_BINDING_INDEX);
	for (auto t : texs) {
		t.second->bind(GL_TEXTURE0 + t.first);
	}
}


void Material::unuse() {
	for (auto t : texs) {
		t.second->unbind();
	}
}