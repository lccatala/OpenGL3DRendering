

#ifndef _BONES_H
#define _BONES_H 2019

#include <memory>
#include <vector>
#include <glm/fwd.hpp>
#include "uniformBufferObject.h"

namespace PGUPV {
  /* Clase que representa el conjunto de huesos de una malla animada. Es un bloque de 
  uniforms que se puede usar en tus shaders (igual que has estado utilizando GLMatrices 
  hasta ahora).
  Para incluir la definición de los huesos, introduce la siguiente línea en el código
  de tus shaders:

  $Bones

  Dicha línea será sustituída por el siguiente bloque antes de la compilación
  del shader:

  layout (std140) uniform Bones {
	mat4 bones[];
  }
  
  */


  class UBOBones : public UniformBufferObject, public std::enable_shared_from_this<UBOBones> {
  public:
    virtual ~UBOBones() {};
    const static std::string blockName;
    const static Strings definition;
	const std::string &getBlockName() const override {
		return blockName;
	};
	const Strings &getDefinition() const override {
		return definition;
	}
	static std::shared_ptr<UBOBones> build(const std::vector<glm::mat4> &b);
	static std::shared_ptr<UBOBones> build();

	const std::vector<glm::mat4> &getBones() { return bones; }
	
	//! Establece estos huesos para usarlos en las siguientes llamadas de dibujo
	void use();
	void unuse();

	static const uint32_t MAX_BONES = 150U;
  private:
    UBOBones(unsigned long sizeInBytes);
	UBOBones(UBOBones &m) = delete;
	UBOBones & operator=(const UBOBones &m) = delete;
	std::vector<glm::mat4> bones;
  };
};


#endif

