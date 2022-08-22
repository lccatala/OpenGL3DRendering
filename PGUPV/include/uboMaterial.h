

#ifndef _MATERIAL_H
#define _MATERIAL_H 2011

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "uniformBufferObject.h"

namespace PGUPV {

  struct MaterialMembers {
    MaterialMembers(const glm::vec4 &ambient = glm::vec4(0.2, 0.2, 0.2, 1.0),
      const glm::vec4 &diffuse = glm::vec4(0.8, 0.8, 0.8, 1.0),
      const glm::vec4 &specular = glm::vec4(0.0f),
      const glm::vec4 &emissive = glm::vec4(0.0f),
      GLfloat shininess = 0.0,
      GLint textureCount = 0) {
      this->diffuse = diffuse;
      this->ambient = ambient;
      this->specular = specular;
      this->emissive = emissive;
      this->shininess = shininess;
      this->textureCount = textureCount;
      for (uint i = 0; i < sizeof(glm::vec4) - sizeof(GLfloat) - sizeof(GLint); i++)
        padding[i] = 0;
    }
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular;
    glm::vec4 emissive;
    GLfloat shininess;
    GLuint textureCount;
    char padding[sizeof(glm::vec4) - sizeof(GLfloat) - sizeof(GLint)];
  };


  /* Clase que representa un material. Es un bloque de uniforms que se puede
  usar en tus shaders (igual que has estado utilizando GLMatrices hasta ahora).
  Para incluir la definición del material, introduce la siguiente línea en el código
  de tus shaders:

  $Material

  Dicha línea será sustituída por el siguiente bloque antes de la compilación
  del shader:

  layout (std140) uniform Material {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    uint textureCount;
  }

  Luego, puedes modificar la definición del material desde tu programa C++ de varias formas:

  -Al crear el objeto UBOMaterial, pasando un objeto Material (que puedes crear tú, o usar uno
    de los predefinidos en stockMaterials.h
  -Al crear el objeto UBOMaterial, pasar las propiedades del material
  -A través de los 'setters'
  -La variable textureCount es una máscara de bits, que contiene el número de texturas de cada clase:
    0b111: contador de texturas difusas
    0b111000: contador de texturas especulares
    0b111000000: contador de texturas de mapas de normales
    0b111000000000: contador de texturas de mapas de alturas
    0b111000000000000: contador de texturas de opacidad

    Junto a la definición del bloque se acompañan funciones de acceso para obtener los contadores: numDiffTextures(),
    numSpecTextures(), etc.
  */


  class UBOMaterial : public UniformBufferObject, public std::enable_shared_from_this<UBOMaterial> {
  public:
    virtual ~UBOMaterial() {};
    const static std::string blockName;
    const static Strings definition;
    const std::string &getBlockName() const override;
    const Strings &getDefinition() const override;
    static std::shared_ptr<UBOMaterial> build(const MaterialMembers &m);
    static std::shared_ptr<UBOMaterial> build(
      const glm::vec4 &ambient = glm::vec4(0.5, 0.5, 0.5, 1.0),
      const glm::vec4 &diffuse = glm::vec4(0.8, 0.1, 0.1, 1.0),
      const glm::vec4 &specular = glm::vec4(0.9, 0.9, 0.9, 1.0),
      const glm::vec4 &emissive = glm::vec4(0.0f),
      const GLfloat shininess = 100.0,
      const GLuint texturesCount = 0);
    const MaterialMembers &getMaterial() { return info; }
    void setMaterial(const MaterialMembers &m);
  private:
    UBOMaterial();
    // Cuidado! El orden y el tipo de los miembros del tipo Material DEBEN COINCIDIR
    // con los especificados en la función getDefinition!!
    MaterialMembers info;
    UBOMaterial(UBOMaterial &m) = delete;
	UBOMaterial & operator=(const UBOMaterial &m) = delete;
  };
};


#endif

