#pragma once

namespace PGUPV {
	class UBOLightSources;
	class GLMatrices;

	/**
		Dibuja las fuentes activas del objeto indicado
		\param glMats las matrices que se están utilizando para dibujar la escena
		\param lights la estructura con las fuentes a dibujar
	*/
	void renderLightSources(GLMatrices &glMats, const UBOLightSources &lights);
};