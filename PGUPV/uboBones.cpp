#include "log.h"
#include "uboBones.h"
#include "indexedBindingPoint.h"

#include <gsl/gsl>

using PGUPV::UBOBones;

const std::string UBOBones::blockName{ "Bones" };
const Strings UBOBones::definition{
	  "layout(std140, binding=" + std::to_string(UBO_BONES_BINDING_INDEX) + ") uniform Bones {",
	  "  mat4 bones[" + std::to_string(UBOBones::MAX_BONES) + "];",
	  "};"
};

UBOBones::UBOBones(unsigned long sizeInBytes) : UniformBufferObject(sizeInBytes) {
}


std::shared_ptr<UBOBones> UBOBones::build(const std::vector<glm::mat4> &b) {
	if (b.size() > MAX_BONES) {
		ERRT("El número máximo de huesos soportado es " + std::to_string(MAX_BONES));
	}
	auto ubo = build();

	ubo->bones = b;
	ubo->use();
	PGUPV::gl_uniform_buffer.write((void *)&b[0], gsl::narrow<uint32_t>(sizeof(glm::mat4) * b.size()), 0);
	return ubo;
}

std::shared_ptr<UBOBones> UBOBones::build()
{
	std::shared_ptr<UBOBones> ubo = std::shared_ptr<UBOBones>(new UBOBones(sizeof(glm::mat4) * MAX_BONES));
	allocate(ubo);
	ubo->setGlDebugLabel(ubo->getBlockName());
	INFO("UBO Bones creado");

	return ubo;
}

void UBOBones::use() {
	PGUPV::gl_uniform_buffer.bindBufferBase(shared_from_this(), UBO_BONES_BINDING_INDEX);
}

void UBOBones::unuse() {
}
