#pragma once

#include <memory>
#include <map>
#include <glm/vec4.hpp>

#include "baseMaterial.h"

namespace PGUPV {
	class BindableTexture;
	class UBOPBRMaterial;

	class PBRMaterial : public BaseMaterial {
	public:
		// Cuidado! No cambiar de orden. Ver Material::setTexture
		enum TextureType {
			BASECOLOR_TUNIT = 0,
			NORMAL_TUNIT = 4,
			EMISSION_TUNIT = 8,
			METALNESS_TUNIT = 12,
			ROUGHNESS_TUNIT = 16,
			AMBIENTOCLUSSION_TUNIT = 20
		};


		PBRMaterial();
		PBRMaterial(const std::string &name);

		//! Establece este material para usarlo en las siguientes llamadas de dibujo
		void use() override;
		//! Desvincula las texturas que se asociaron con este material
		void unuse() override;

		void setBaseColorTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		void setNormalMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		void setEmissionTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		void setMetalnessTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		void setRoughnessTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		void setAmbientOcTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);

		/**
		Devuelve el número de texturas usadas del tipo indicado
		*/
		unsigned int getTextureCount(TextureType type);

		/**
		\return el número total de texturas usadas por la malla
		*/
		size_t getTextureCount() const { return texs.size(); }

		/**
		\return el contador de cada tipo de textura
		*/
		unsigned int getTextureCounters() const;
		/**
		Establecer el identificador de textura y la unidad de textura donde se vinculará
			\param tex_unit: 0, 1, 2...
			\param tex: puntero al objeto textura
			\warning Es preferible que uses las otras llamads de set*Texture
		*/
		void setTexture(unsigned int tex_unit, std::shared_ptr<BindableTexture> tex);

		/**
		Devuelve la textura asociada a la unidad de textura indicada
		\param tex_unit: 0, 1, 2...
		\return puntero al objeto textura
		*/
		std::shared_ptr<BindableTexture> getTexture(unsigned int tex_unit);

	private:
		std::shared_ptr<UBOPBRMaterial> uboPBRMaterial;
		std::map<unsigned int, std::shared_ptr<BindableTexture> > texs;

		void setTextureCount(TextureType type, unsigned int count);
	};
};