
#include <glm/glm.hpp>

#include "indexedBindingPoint.h"
#include "uboLightSources.h"
#include "utils.h"
#include "log.h"

using PGUPV::UBOLightSources;
using PGUPV::LightSourceParameters;
using PGUPV::LightSources;

UBOLightSources::UBOLightSources() : UniformBufferObject(sizeof(info)) {
	// Todas las luces con los valores por defecto, pero sólo está activa la
	// primera
	for (uint i = 1; i < NUM_LIGHT_SOURCES; i++)
		info.ls[i].enabled = false;
}

const std::string UBOLightSources::blockName{ "Lights" };
const Strings UBOLightSources::definition{
	"struct LightSource {",
	"  vec4 ambient, diffuse, specular;",
	"  vec4 positionWorld;",
	"  vec4 positionEye;",
	"  vec3 spotDirectionWorld;",
	"  int directional;",
	"  vec3 spotDirectionEye;",
	"  int enabled;",
	"  float spotExponent, spotCutoff, spotCosCutoff;",
	"  vec3 attenuation;",
	"};",

	"layout (std140, binding=" + std::to_string(UBO_LIGHTS_BINDING_INDEX) + ") uniform Lights {",
	"  LightSource lights[" + std::to_string(NUM_LIGHT_SOURCES) +
		"];",
	"};"
};

const std::string &UBOLightSources::getBlockName() const {
	return UBOLightSources::blockName;
}

const Strings &UBOLightSources::getDefinition() const {
	return UBOLightSources::definition;
}

void UBOLightSources::setLightSource(uint index,
	const LightSourceParameters &lsp) {
	if (index >= NUM_LIGHT_SOURCES)
		ERRT("Luz no existente");

	info.ls[index] = lsp;

	sync(index);
}

LightSourceParameters UBOLightSources::getLightSource(uint index) const {
	if (index >= NUM_LIGHT_SOURCES)
		ERRT("Luz no existente");

	return info.ls[index];
}

void UBOLightSources::sync(int light) {

	PGUPV::gl_uniform_buffer.bind(shared_from_this());

	if (light >= 0) {
		PGUPV::gl_uniform_buffer.write((void *)&info.ls[light],
			sizeof(LightSourceParameters),
			sizeof(LightSourceParameters) * light);
	}
	else {
		PGUPV::gl_uniform_buffer.write((void *)&info, sizeof(LightSources), 0);
	}
}

std::shared_ptr<UBOLightSources> UBOLightSources::build() {
	std::shared_ptr<UBOLightSources> ubo =
		std::shared_ptr<UBOLightSources>(new UBOLightSources());
	allocate(ubo);

	ubo->setGlDebugLabel(ubo->getBlockName());
	ubo->sync();
	INFO("UBO LightSources creado");
	return ubo;
}

void UBOLightSources::switchLight(uint light, bool on) {
	assert(light <= NUM_LIGHT_SOURCES);
	info.ls[light].enabled = on;
	sync(light);
}

void UBOLightSources::switchAll(bool on) {
	for (uint i = 0; i < NUM_LIGHT_SOURCES; ++i) {
		info.ls[i].enabled = on;
	}
	sync();
}

std::ostream &PGUPV::operator<<(std::ostream &os, const LightSources &ls) {
	os << "Cantidad de fuentes: " << NUM_LIGHT_SOURCES << std::endl;
	for (uint i = 0; i < NUM_LIGHT_SOURCES; ++i) {
		os << "Light " << i << std::endl;
		os << ls.ls[i];
	}
	return os;
}

std::ostream &PGUPV::operator<<(std::ostream &os,
	const LightSourceParameters &lsp) {
	os << "enabled: " << lsp.enabled << std::endl;
	os << "ambient: " << PGUPV::to_string(lsp.ambient, 1) << std::endl;
	os << "diffuse: " << PGUPV::to_string(lsp.diffuse, 1) << std::endl;
	os << "specular: " << PGUPV::to_string(lsp.specular, 1) << std::endl;
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
