
#ifndef _UBOPBRLIGHTSOURCE_H
#define _UBOPBRLIGHTSOURCE_H 2021

#ifndef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#endif
#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "uniformBufferObject.h"

namespace PGUPV {
	/**
	\struct PBRLightSourceParameters
	Mantiene las características de una fuente de luz. Una fuente de luz se puede
	encender o apagar.

	*/

	struct PBRLightSourceParameters {
		/**
		Constructor
		\param color Color de la fuente
		\param position Posición de la luz en el sistema de coordenadas del mundo. Si
		tu shader necesita la posición en el sistema de coordenadas de la cámara,
		recuerda actualizar el campo positionEye antes de dibujar cada frame
		\param directional true si la luz es direccional
		\param spotDirection Dirección del foco en el sistema de coordenadas del
		mundo. Si tu shader necesita la dirección en el sistema de coordenadas de la
		cámara, recuerda actualizar el campo spotDirectionEye antes de dibujar cada
		frame
		\param spotExponent Exponente especular (por defecto, 50)
		\param spotCutoff Ángulo de apertura del foco (por defecto, 180.0, es decir,
		no hay foco)
		\param attenuation Coeficientes de atenuación
		\param enabled true si esta luz está encendida
		*/
		PBRLightSourceParameters(
			const glm::vec3 &color = glm::vec3(0.8f, 0.8f, 0.8f),
			const float intensity = 1.0f,
			const glm::vec4 &position = glm::vec4(0.0, 0.0, 0.0, 1.0),
			bool directional = false,
			const glm::vec3 &spotDirection = glm::vec3(0.0, 0.0, -1.0),
			float spotExponent = 50.0, float spotCutoff = 180.0,
			const glm::vec3 &attenuation = glm::vec3(1.0, 0.0, 0.0),
			bool enabled = true) {
			this->color = color;
			this->intensity = intensity;
			this->scaledColor = color * intensity;
			this->positionWorld = position;
			this->spotDirectionWorld = spotDirection;
			this->attenuation = attenuation;
			this->spotExponent = spotExponent;
			this->spotCutoff = spotCutoff;
			this->spotCosCutoff = (float)cos(M_PI * spotCutoff / 180.0);
			this->enabled = enabled;
			this->directional = directional;
			for (uint i = 0; i < sizeof(_pad1); ++i)
				_pad1[i] = 0;
			for (uint i = 0; i < sizeof(_pad2); ++i)
				_pad2[i] = 0;
			for (uint i = 0; i < sizeof(_pad3); ++i)
				_pad3[i] = 0;
		}
		glm::vec3 color;
		float intensity;
		glm::vec4 positionWorld;
		glm::vec4 positionEye;
		glm::vec3 spotDirectionWorld;
		GLint directional;
		glm::vec3 spotDirectionEye;
		GLint enabled;
		GLfloat spotExponent, spotCutoff, spotCosCutoff;
		unsigned char _pad1[sizeof(glm::vec4) - 3 * sizeof(GLfloat)];
		// .x: constant, .y: linear, .z: quadratic
		glm::vec3 attenuation;
		unsigned char _pad2[sizeof(glm::vec4) - sizeof(glm::vec3)];
		glm::vec3 scaledColor; 
		unsigned char _pad3[sizeof(glm::vec4) - sizeof(glm::vec3)];
	private:
		friend std::ostream &operator<<(std::ostream &os,
			const PBRLightSourceParameters &lsp);
	};
	std::ostream &operator<<(std::ostream &os, const PBRLightSourceParameters &lsp);

#define NUM_LIGHT_SOURCES 4

	struct PBRLightSources {
		PBRLightSourceParameters ls[NUM_LIGHT_SOURCES];

	private:
		friend std::ostream &operator<<(std::ostream &os, const PBRLightSources &ls);
	};
	std::ostream &operator<<(std::ostream &os, const PBRLightSources&ls);

	/**
	\class UBOPBRLightSources
	Esta clase mantiene las luces de una escena. Es un Uniform Buffer Object que
	facilita el acceso y la modificación de los parámetros de cada fuente de luz.
	Para usarlo en tus shaders, sólo tienes que incluir:

	$PBRLights

	en el shader, y crear y vincular un objeto UBOPBRLightSources a tu programa:

	std::shared_ptr<UBOPBRLightSources> luces;

	luces = UBOPBRLightSources::build();
	gshader.connectUniformBlock(luces, UBO_PBR_LIGHTS_BINDING_INDEX);
	gshader.loadFiles(...);
	gshader.compile();

	Un bloque de luces soporta NUM_LIGHT_SOURCES, y cuando se crea sólo la primera
	está encendida.

	*/

	class UBOPBRLightSources : public UniformBufferObject,
		public std::enable_shared_from_this<UBOPBRLightSources> {
	public:
		/**
		Factoría de objetos UBOPBRLightSources
		\return Devuelve un puntero inteligente a un bloque de fuentes de luz. Por
		defecto hay una luz difusa, en la posición de la cámara.
		*/
		static std::shared_ptr<UBOPBRLightSources> build();

		/**
		Reemplaza la luz de la posición indicada
		\param index Índice de la luz a reemplazar (desde cero a MAX_LIGHT_SOURCES-1)
		\param lsp Características de la fuente
		*/
		void setLightSource(uint index, const PBRLightSourceParameters &lsp);
		/**
		Obtiene la luz indicada (para modificarla, tendrás que volver a pasarla con
		setLightSource)
		\param index Índice de la luz a modificar
		*/
		const PBRLightSourceParameters &getLightSource(uint index) const;
		/**
		Enciende o apaga una fuente
		\param light el índice de la luz a encender/apagar
		\param on si true, enciende, si falsa, apaga
		*/
		void switchLight(uint light, bool on);

		/**
		 Enciende o apaga todas las fuentes
		 \param on si true, enciende, si falsa, apaga
		 */
		void switchAll(bool on);

		/**
		 Devuelve si la luz está encendida
		 \return true si la luz está encendida
		 */
		bool isOn(uint light) const { return info.ls[light].enabled != GL_FALSE; };

		/**
		 Devuelve el número de fuentes en el bloque (siempre NUM_LIGHT_SOURCES). Cada
		 una de ellas puede estar encendida o apagada.
		 \return el número de luces definidas (siempre devuelve NUM_LIGHT_SOURCES).
		 */
		uint size() const { return NUM_LIGHT_SOURCES; };

		void updateLightEyeSpacePosition(const glm::mat4& viewMatrix);
		const std::string &getBlockName() const override;
		const Strings &getDefinition() const override;
		static const std::string blockName;
		static const Strings definition;
	private:
		/**
		 Escribe el valor de la variable correspondiente a la luz dada al buffer
		 object.
		 \param light índice de la luz a volcar al buffer object, o -1 para volcar
		 todas las luces
		 */
		void sync(int light = -1);
		UBOPBRLightSources();
		PBRLightSources info;
	};
} // namespace PGUPV;

#endif
