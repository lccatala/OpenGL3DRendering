

#include <glm/glm.hpp>

#include "uboPBRMaterial.h"
#include "indexedBindingPoint.h"
#include "log.h"

using PGUPV::PBRMaterialMembers;
using PGUPV::UBOPBRMaterial;

UBOPBRMaterial::UBOPBRMaterial() : UniformBufferObject(sizeof(info)) {}

const std::string UBOPBRMaterial::blockName{ "PBRMaterial" };
const Strings UBOPBRMaterial::definition{
	"layout (std140, binding=" + std::to_string(UBO_PBR_MATERIALS_BINDING_INDEX) + ") uniform PBRMaterial {",
	"int textureCount;",
	"};",
	"",
	"uint numBaseColorTextures() { return bitfieldExtract(textureCount, 0, 3); }",
	"uint numNormalMapTextures() { return bitfieldExtract(textureCount, 3, 3); }",
	"uint numEmissionTextures() { return bitfieldExtract(textureCount, 6, 3); }",
	"uint numMetalnessTextures() { return bitfieldExtract(textureCount, 9, 3); }",
	"uint numRoughnessTextures() { return bitfieldExtract(textureCount, 12, 3); }",
	"uint numAmbientOcTextures() { return bitfieldExtract(textureCount, 15, 3); }",
	"layout (binding=0) uniform sampler2D baseColorMap;",
	"layout (binding=1) uniform sampler2D baseColorMap1;",
	"layout (binding=4) uniform sampler2D normalMap;",
	"layout (binding=5) uniform sampler2D normalMap1;",
	"layout (binding=8) uniform sampler2D emissionMap;",
	"layout (binding=9) uniform sampler2D emissionMap1;",
	"layout (binding=12) uniform sampler2D metalnessMap;",
	"layout (binding=13) uniform sampler2D metalnessMap1;",
	"layout (binding=16) uniform sampler2D roughnessMap;",
	"layout (binding=17) uniform sampler2D roughnessMap1;",
	"layout (binding=20) uniform sampler2D ambientOcMap;",
	"layout (binding=21) uniform sampler2D ambientOcMap1;",

};


const std::string &UBOPBRMaterial::getBlockName() const {
	return UBOPBRMaterial::blockName;
}

const Strings &UBOPBRMaterial::getDefinition() const {
	return UBOPBRMaterial::definition;
}

void UBOPBRMaterial::setMaterial(const PBRMaterialMembers &m) {
	info = m;
	PGUPV::gl_uniform_buffer.bind(shared_from_this());
	PGUPV::gl_uniform_buffer.write((void *)&info);
}

std::shared_ptr<UBOPBRMaterial> UBOPBRMaterial::build(const PBRMaterialMembers &m) {
	std::shared_ptr<UBOPBRMaterial> ubo =
		std::shared_ptr<UBOPBRMaterial>(new UBOPBRMaterial());
	allocate(ubo);
	ubo->setGlDebugLabel(ubo->getBlockName());
	ubo->setMaterial(m);
	INFO("UBO PBRMaterial creado");
	return ubo;
}

std::shared_ptr<UBOPBRMaterial> PGUPV::UBOPBRMaterial::build()
{
	return build(PBRMaterialMembers{});
}
