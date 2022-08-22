#include "texture2DBlitter.h"

#include "app.h"
#include "texture2D.h"
#include "stockModels.h"
#include "stockPrograms.h"
#include "indexedBindingPoint.h"
#include "glMatrices.h"
#include "glStateCache.h"

void PGUPV::texture2DBlit(const glm::vec3 &pos, float height, std::shared_ptr<Texture2D> texture) {
	static PGUPV::Rect rect;

	auto tu = PGUPV::App::getScratchUnitTextureNumber();
	auto prevProg = PGUPV::TextureReplaceProgram::use();
	auto old = PGUPV::TextureReplaceProgram::setTextureUnit(tu);
	glActiveTexture(GL_TEXTURE0 + tu);
	texture->bind(GL_TEXTURE0 + tu);
	auto mats = std::dynamic_pointer_cast<PGUPV::GLMatrices>(gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX));

	PGUPV::GLStateCapturer<PGUPV::PolygonModeState> polygonMode;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->loadIdentity(GLMatrices::MODEL_MATRIX);
	mats->translate(GLMatrices::MODEL_MATRIX, pos);
	mats->scale(GLMatrices::MODEL_MATRIX, texture->getWidth() * height/texture->getHeight(), height, 1.0f);
	rect.render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	if (tu != old) PGUPV::TextureReplaceProgram::setTextureUnit(old);
	prevProg->use();

	polygonMode.restore();
}
