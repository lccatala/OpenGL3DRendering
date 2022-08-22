

#include <glm/glm.hpp>

#include "uboMaterial.h"
#include "indexedBindingPoint.h"
#include "log.h"

using PGUPV::MaterialMembers;
using PGUPV::UBOMaterial;

UBOMaterial::UBOMaterial() : UniformBufferObject(sizeof(info)) {}

const std::string UBOMaterial::blockName{ "Material" };
const Strings UBOMaterial::definition{
	"layout (std140, binding=" + std::to_string(UBO_MATERIALS_BINDING_INDEX) + ") uniform Material {",
	"vec4 diffuse;",
	"vec4 ambient;",
	"vec4 specular;",
	"vec4 emissive;",
	"float shininess;",
	"int textureCount;",
	"};",
	"",
	"uint numDiffTextures() { return bitfieldExtract(textureCount, 0, 3); }",
	"uint numSpecTextures() { return bitfieldExtract(textureCount, 3, 3); }",
	"uint numNormTextures() { return bitfieldExtract(textureCount, 6, 3); }",
	"uint numHeightTextures() { return bitfieldExtract(textureCount, 9, 3); }",
	"uint numOpacTextures() { return bitfieldExtract(textureCount, 12, 3); }",
	"uint numAmbTextures() { return bitfieldExtract(textureCount, 15, 3); }"
};


const std::string &UBOMaterial::getBlockName() const {
	return UBOMaterial::blockName;
}

const Strings &UBOMaterial::getDefinition() const {
	return UBOMaterial::definition;
}

void UBOMaterial::setMaterial(const MaterialMembers &m) {
	info = m;
	PGUPV::gl_uniform_buffer.bind(shared_from_this());
	PGUPV::gl_uniform_buffer.write((void *)&info);
}

std::shared_ptr<UBOMaterial> UBOMaterial::build(const MaterialMembers &m) {
	std::shared_ptr<UBOMaterial> ubo =
		std::shared_ptr<UBOMaterial>(new UBOMaterial());
	allocate(ubo);
	ubo->setGlDebugLabel(ubo->getBlockName());
	ubo->setMaterial(m);
	INFO("UBO Material creado");
	return ubo;
}

std::shared_ptr<UBOMaterial> UBOMaterial::build(const glm::vec4 &ambient, const glm::vec4 &diffuse,
	const glm::vec4 &specular, const glm::vec4 &emissive,
	const GLfloat shininess, const GLuint textureCount) {
	return build(
		MaterialMembers(ambient, diffuse, specular, emissive, shininess, textureCount));
}
