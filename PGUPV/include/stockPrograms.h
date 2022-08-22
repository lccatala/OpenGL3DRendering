#pragma once
// 2016

#include <memory>
#include <log.h>
#include "program.h"
#include "lifetimeManager.h"

namespace PGUPV {
	//void builder(StockPrograms stockProgramId, Program &p);
	void buildConstantShading(PGUPV::Program &program);
	void buildReplaceTexture(PGUPV::Program &program);
	void buildConstantColorUniform(PGUPV::Program &program);


  template <void (*builder)(Program &)>
  class StockProgram {
  public:
    /**
    Instala el programa en la GPU
    */
    static Program *use() {
      init();
      return instance->program.use();
    }
    /**
    \return una referencia al programa.
    */
    static Program &getProgram() {
      init();
      return instance->program;
    }
  protected:
    // No puedes instanciar objetos de esta clase. 
    StockProgram() {
		INFO("StockProgram creado");
	};
    static void init() {
      if (!instance) {
        instance = new StockProgram<builder>();
        builder(instance->program);
		SetLongevity<StockProgram<builder>>(instance, 5, Private::Deleter<StockProgram<builder>>::Delete);
      }
    }
    static StockProgram<builder> *instance;
    Program program;
  };

  template <void(*builder)(Program &)>
  StockProgram<builder> *StockProgram<builder>::instance;

  /**
  \class ConstantIllumProgram
  Clase que encapsula un programa que dibuja las primitivas interpolando el color
  de los vértices (las primitivas dibujadas deben definir un color por vértice).
  No tienes que instanciar objetos, sólo tienes que llamar al método use() para
  instalar el programa:

  ConstantIllumProgram::use();

  model.render();

  */


  typedef StockProgram<buildConstantShading> ConstantIllumProgram;

  /**
  \class TextureReplaceProgram
  Programa que aplica al polígono dibujado una textura tal cual, según las
  coordenadas de textura establecidas por el polígono.
  No tienes que instanciar objetos, sólo tienes que llamar al método use() para
  instalar el programa. El método setTextureUnit define en qué unidad de textura
  está la textura a utilizar.

  TextureReplaceProgram::use();
  TextureReplaceProgram::setTextureUnit(2);

  model.render();
  */
  class TextureReplaceProgram {
  public:
    /**
    Instala el programa en la GPU
    */
    static Program *use();
    /**
    Establece la unidad de textura donde se encuentra la textura a dibujar
    \param texUnit la unidad de textura a usar (p.e., 0 para GL_TEXTURE0)
    \return la unidad de textura que se estaba usando
    \warning El programa debe estar instalado en la GPU (con 'use' *antes* de llamar a este
    método)
    \warning ¡Cuidado! Sólo hay una instancia de este programa para toda la aplicación,
    por lo que si cambias la unidad de textura y usas el programa en varios sitios de
    tu aplicación, todos ellos usarán la nueva unidad, hasta que haya un nuevo cambio.
    */
    static int setTextureUnit(int texUnit);
  private:
    static int currentTexUnit;
  };



  /**
  \class ConstantUniformColorProgram
  Programa que aplica a la primitiva un color constante especificado en tiempo 
  de ejecución.
  No tienes que instanciar objetos, sólo tienes que llamar al método use() para
  instalar el programa. El método setColor define el color a utilizar.

  ConstantUniformColorProgram::use();
  ConstantUniformColorProgram::setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

  model.render();
  */
  class ConstantUniformColorProgram {
  public:
	  /**
	  Instala el programa en la GPU
	  */
	  static Program *use();
	  /**
	  Establece el color que se usará para dibujar las primitivas
	  \param color el color
	  \warning El programa debe estar instalado en la GPU (con 'use' *antes* de llamar a este
	  método)
	  */
	  static void setColor(const glm::vec4 &color);
  private:
	  static GLint colorLoc;
  };
};
