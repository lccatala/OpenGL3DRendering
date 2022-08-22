#pragma once

#include <memory>
#include <map>
#include <glm/vec4.hpp>

#include "baseMaterial.h"

namespace PGUPV {
	class BindableTexture;
	class UBOMaterial;

	class Material : public BaseMaterial {
	public:
		// Cuidado! No cambiar de orden. Ver Material::setTexture
		enum TextureType {
			DIFFUSE_TUNIT = 0,
			SPECULAR_TUNIT = 4,
			NORMALMAP_TUNIT = 8,
			HEIGHTMAP_TUNIT = 12,
			OPACITYMAP_TUNIT = 16,
			AMBIENT_TUNIT = 20
		};


		Material();
		Material(const std::string &name);
		Material(const std::string &name, const glm::vec4 &amb, const glm::vec4 &dif,
			const glm::vec4 &spec, const glm::vec4 &emi, float shininess);

		//! Establece este material para usarlo en las siguientes llamadas de dibujo
		void use() override;
		//! Desvincula las texturas que se asociaron con este material
		void unuse() override;

		/**
		Establece una textura de tipo difuso
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setDiffuseTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		/**
		Establece una textura de tipo especular
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setSpecularTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);

		/**
		Establece una textura de tipo mapa de normales
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setNormalMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		/**
		Establece una textura de tipo mapa de alturas
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setHeightMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		/**
		Establece una textura de tipo mapa de opacidad
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setOpacitytMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);
		/**
		Establece una textura de tipo mapa de oclusión ambiental
		\param tex El objeto textura
		\param index el índice dentro de las texturas de este tipo
		*/
		void setAmbientMapTexture(std::shared_ptr<BindableTexture> tex, unsigned int index = 0);

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

		const glm::vec4 &getAmbient() const;
		const glm::vec4 &getDiffuse() const;
		const glm::vec4 &getSpecular() const;
		const glm::vec4 &getEmissive() const;
		float getShininess() const;

		void setAmbient(const glm::vec4 &c);
		void setDiffuse(const glm::vec4 &c);
		void setSpecular(const glm::vec4 &c);
		void setEmissive(const glm::vec4 &c);
		void setShininess(float s);

	private:
		std::shared_ptr<UBOMaterial> ubomaterial;
		std::map<unsigned int, std::shared_ptr<BindableTexture> > texs;

		void setTextureCount(TextureType type, unsigned int count);
	};
};