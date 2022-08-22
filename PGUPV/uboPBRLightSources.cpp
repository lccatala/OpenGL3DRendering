
#include <glm/glm.hpp>

#include "indexedBindingPoint.h"
#include "uboPBRLightSources.h"
#include "utils.h"
#include "log.h"

using PGUPV::UBOPBRLightSources;
using PGUPV::PBRLightSourceParameters;
using PGUPV::PBRLightSources;

UBOPBRLightSources::UBOPBRLightSources() : UniformBufferObject(sizeof(info)) {
	// Todas las luces con los valores por defecto, pero sólo está activa la
	// primera
	for (uint i = 1; i < NUM_LIGHT_SOURCES; i++)
		info.ls[i].enabled = false;
}

const std::string UBOPBRLightSources::blockName{ "PBRLights" };
const Strings UBOPBRLightSources::definition{
	"struct PBRLightSource {",
	"  vec3 color;",
	"  float intensity;",
	"  vec4 positionWorld;",
	"  vec4 positionEye;",
	"  vec3 spotDirectionWorld;",
	"  int directional;",
	"  vec3 spotDirectionEye;",
	"  int enabled;",
	"  float spotExponent, spotCutoff, spotCosCutoff;",
	"  vec3 attenuation;",
	"  vec3 scaledColor;"
	"};",

	"layout (std140, binding=" + std::to_string(UBO_PBR_LIGHTS_BINDING_INDEX) + ") uniform PBRLights {",
	"  PBRLightSource lights[" + std::to_string(NUM_LIGHT_SOURCES) +
		"];",
	"};"
};

const std::string &UBOPBRLightSources::getBlockName() const {
	return UBOPBRLightSources::blockName;
}

const Strings &UBOPBRLightSources::getDefinition() const {
	return UBOPBRLightSources::definition;
}

void UBOPBRLightSources::setLightSource(uint index,
	const PBRLightSourceParameters &lsp) {
	if (index >= NUM_LIGHT_SOURCES)
		ERRT("Luz no existente");

	info.ls[index] = lsp;

	sync(index);
}

const PBRLightSourceParameters &UBOPBRLightSources::getLightSource(uint index) const {
	if (index >= NUM_LIGHT_SOURCES)
		ERRT("Luz no existente");

	return info.ls[index];
}

void UBOPBRLightSources::sync(int light) {

	PGUPV::gl_uniform_buffer.bind(shared_from_this());

	if (light >= 0) {
		PGUPV::gl_uniform_buffer.write((void *)&info.ls[light],
			sizeof(PBRLightSourceParameters),
			sizeof(PBRLightSourceParameters) * light);
	}
	else {
		PGUPV::gl_uniform_buffer.write((void *)&info, sizeof(PBRLightSources), 0);
	}
}

std::shared_ptr<UBOPBRLightSources> UBOPBRLightSources::build() {
	std::shared_ptr<UBOPBRLightSources> ubo =
		std::shared_ptr<UBOPBRLightSources>(new UBOPBRLightSources());
	allocate(ubo);

	ubo->setGlDebugLabel(ubo->getBlockName());
	ubo->sync();
	INFO("UBO PBRLightSources creado");
	return ubo;
}

void UBOPBRLightSources::switchLight(uint light, bool on) {
	assert(light < NUM_LIGHT_SOURCES);
	info.ls[light].enabled = on;
	sync(light);
}



void UBOPBRLightSources::switchAll(bool on) {
	for (uint i = 0; i < NUM_LIGHT_SOURCES; ++i) {
		info.ls[i].enabled = on;
	}
	sync();
}


void UBOPBRLightSources::updateLightEyeSpacePosition(const glm::mat4& viewMatrix) {
	for (uint i = 0; i < NUM_LIGHT_SOURCES; ++i) {
		info.ls[i].positionEye = viewMatrix * info.ls[i].positionWorld;
	}
	sync();
}

std::ostream &PGUPV::operator<<(std::ostream &os, const PBRLightSources &ls) {
	os << "Cantidad de fuentes: " << NUM_LIGHT_SOURCES << std::endl;
	for (uint i = 0; i < NUM_LIGHT_SOURCES; ++i) {
		os << "Light " << i << std::endl;
		os << ls.ls[i];
	}
	return os;
}

std::ostream &PGUPV::operator<<(std::ostream &os,
	const PBRLightSourceParameters &lsp) {
	os << "enabled: " << lsp.enabled << std::endl;
	os << "scaledColor: " << PGUPV::to_string(lsp.scaledColor, 1) << std::endl;
	os << "color: " << PGUPV::to_string(lsp.color, 1) << std::endl;
	os << "intensity: " << PGUPV::to_string(lsp.intensity, 1) << std::endl;
	os << "directional: " << lsp.directional << std::endl;
	os << "positionWorld: " << PGUPV::to_string(lsp.positionWorld, 1)
		<< std::endl;
	os << "positionEye: " << PGUPV::to_string(lsp.positionEye, 1) << std::endl;
	os << "spotDirectionWorld: " << PGUPV::to_string(lsp.spotDirectionWorld, 1)
		<< std::endl;
	os << "spotDirectionEye: " << PGUPV::to_string(lsp.spotDirectionEye, 1)
		<< std::endl;
	os << "spotExponent: " << lsp.spotExponent << std::endl;
	os << "spotCutoff: " << lsp.spotCutoff << std::endl;
	os << "spotCosCutoff: " << lsp.spotCosCutoff << std::endl;
	os << "attenuation: " << PGUPV::to_string(lsp.attenuation, 1) << std::endl;
	return os;
}
