

#ifndef _PBRMATERIAL_H
#define _PBRMATERIAL_H 2021

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "uniformBufferObject.h"

namespace PGUPV {

	struct PBRMaterialMembers {
		PBRMaterialMembers() : textureCount(0) {
		}
		GLuint textureCount;
	};


	/* Clase que representa un material de tipo PBR. Es un bloque de uniforms que se puede
	usar en tus shaders (igual que has estado utilizando GLMatrices hasta ahora).
	Para incluir la definición del material, introduce la siguiente línea en el código
	de tus shaders:

	$PBRMaterial

	Dicha línea será sustituída por el siguiente bloque antes de la compilación
	del shader:

	(mira UBOPBRMaterial::definition)

	*/


	class UBOPBRMaterial : public UniformBufferObject, public std::enable_shared_from_this<UBOPBRMaterial> {
	public:
		virtual ~UBOPBRMaterial() {};
		const static std::string blockName;
		const static Strings definition;
		const std::string& getBlockName() const override;
		const Strings& getDefinition() const override;
		static std::shared_ptr<UBOPBRMaterial> build(const PBRMaterialMembers& m);
		static std::shared_ptr<UBOPBRMaterial> build();
		const PBRMaterialMembers& getMaterial() { return info; }
		void setMaterial(const PBRMaterialMembers& m);
	private:
		UBOPBRMaterial();
		// Cuidado! El orden y el tipo de los miembros del tipo Material DEBEN COINCIDIR
		// con los especificados en la función getDefinition!!
		PBRMaterialMembers info;
		UBOPBRMaterial(UBOPBRMaterial& m) = delete;
		UBOPBRMaterial& operator=(const UBOPBRMaterial& m) = delete;
	};
};


#endif

