#include "renderHelpers.h"
#include "stockPrograms.h"
#include "stockModels.h"
#include "uboLightSources.h"
#include "glMatrices.h"
#include "indexedBindingPoint.h"

using PGUPV::LightSourceParameters;
using PGUPV::GLMatrices;
using PGUPV::gl_uniform_buffer;
using PGUPV::ConstantUniformColorProgram;

#define CONE_HEIGHT 0.15f
#define CONE_ANGULAR_RADIUS 25.0f
#define POINT_RADIUS 0.1f

void renderSpotLight(GLMatrices &glMats, const glm::vec3 &posWorld, const glm::vec3 &spotDirWorld, const glm::vec4 &lightColor) {
	static PGUPV::Cylinder foco ((float)(2.0 * tan(CONE_ANGULAR_RADIUS * M_PI / 180.0) * CONE_HEIGHT),
		0.00001f, CONE_HEIGHT, 5, 10, glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Dibujamos el foco (sin iluminación)
	glMats.pushMatrix(GLMatrices::MODEL_MATRIX);
	glMats.translate(GLMatrices::MODEL_MATRIX, posWorld);
	glm::vec3 t = -glm::normalize(spotDirWorld);
	float h = 1.0f / (1.0f + t.y);

	glm::mat4 m(t.y + h * t.z * t.z, -t.x, -h * t.z * t.x, 0.0f, t.x, t.y,
		t.z, 0.0f, -h * t.z * t.x, -t.z, t.y + h * t.x * t.x, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glMats.multMatrix(GLMatrices::MODEL_MATRIX, m);
	glMats.translate(GLMatrices::MODEL_MATRIX,
		glm::vec3(0.0, -CONE_HEIGHT / 2, 0.0));
	ConstantUniformColorProgram::setColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	foco.render();

	ConstantUniformColorProgram::setColor(lightColor);
	glMats.scale(GLMatrices::MODEL_MATRIX, glm::vec3(0.99f, 0.99f, 0.99f));
	foco.render();
	glMats.popMatrix(GLMatrices::MODEL_MATRIX);
}


void renderPointLight(GLMatrices &glMats, const glm::vec3 &posWorld, const glm::vec4 &lightColor) {
	static PGUPV::Sphere point(POINT_RADIUS, 10, 10);

	// Dibujamos el foco (sin iluminación)
	glMats.pushMatrix(GLMatrices::MODEL_MATRIX);
	glMats.translate(GLMatrices::MODEL_MATRIX, posWorld);
	ConstantUniformColorProgram::setColor(lightColor);
	point.render();
	glMats.popMatrix(GLMatrices::MODEL_MATRIX);
}


void renderDirectionalLight(GLMatrices &glMats, const glm::vec3 &posWorld, const glm::vec4 &lightColor) {
	static PGUPV::Cylinder foco((float)(tan(CONE_ANGULAR_RADIUS * M_PI / 180.0) * CONE_HEIGHT),
		0.00001f, 2.0f * CONE_HEIGHT, 5, 10, glm::vec4(1.0, 1.0, 1.0, 1.0));


	// Dibujamos el foco (sin iluminación)
	glMats.pushMatrix(GLMatrices::MODEL_MATRIX);
	glMats.translate(GLMatrices::MODEL_MATRIX, posWorld);
	glm::vec3 t = -glm::normalize(posWorld);
	float h = 1.0f / (1.0f + t.y);

	glm::mat4 m(t.y + h * t.z * t.z, -t.x, -h * t.z * t.x, 0.0f, t.x, t.y,
		t.z, 0.0f, -h * t.z * t.x, -t.z, t.y + h * t.x * t.x, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	glMats.multMatrix(GLMatrices::MODEL_MATRIX, m);
	glMats.scale(GLMatrices::MODEL_MATRIX, glm::vec3(0.99f, 0.99f, 0.99f));
	ConstantUniformColorProgram::setColor(lightColor);
	foco.render();
	glMats.popMatrix(GLMatrices::MODEL_MATRIX);
}

void PGUPV::renderLightSources(PGUPV::GLMatrices &glMats, const PGUPV::UBOLightSources &lights)
{
	ConstantUniformColorProgram::use();
	
	for (uint i = 0; i < lights.size(); i++) {
		LightSourceParameters lp = lights.getLightSource(i);
		if (lp.enabled) {
			// FOCO
			if (!lp.directional && lp.spotCutoff < 179.0f) {
				renderSpotLight(glMats, lp.positionWorld, lp.spotDirectionWorld, lp.diffuse);
			}
			else if (lp.directional) {
				// DIRECCIONAL
				renderDirectionalLight(glMats, lp.positionWorld, lp.diffuse);
			}
			else {
				// PUNTUAL
				renderPointLight(glMats, lp.positionWorld, lp.diffuse);
			}
		}
	}
}