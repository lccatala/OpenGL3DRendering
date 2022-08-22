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
  de los v�rtices (las primitivas dibujadas deben definir un color por v�rtice).
  No tienes que instanciar objetos, s�lo tienes que llamar al m�todo use() para
  instalar el programa:

  ConstantIllumProgram::use();

  model.render();

  */


  typedef StockProgram<buildConstantShading> ConstantIllumProgram;

  /**
  \class TextureReplaceProgram
  Programa que aplica al pol�gono dibujado una textura tal cual, seg�n las
  coordenadas de textura establecidas por el pol�gono.
  No tienes que instanciar objetos, s�lo tienes que llamar al m�todo use() para
  instalar el programa. El m�todo setTextureUnit define en qu� unidad de textura
  est� la textura a utilizar.

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
    m�todo)
    \warning �Cuidado! S�lo hay una instancia de este programa para toda la aplicaci�n,
    por lo que si cambias la unidad de textura y usas el programa en varios sitios de
    tu aplicaci�n, todos ellos usar�n la nueva unidad, hasta que haya un nuevo cambio.
    */
    static int setTextureUnit(int texUnit);
  private:
    static int currentTexUnit;
  };



  /**
  \class ConstantUniformColorProgram
  Programa que aplica a la primitiva un color constante especificado en tiempo 
  de ejecuci�n.
  No tienes que instanciar objetos, s�lo tienes que llamar al m�todo use() para
  instalar el programa. El m�todo setColor define el color a utilizar.

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
	  Establece el color que se usar� para dibujar las primitivas
	  \param color el color
	  \warning El programa debe estar instalado en la GPU (con 'use' *antes* de llamar a este
	  m�todo)
	  */
	  static void setColor(const glm::vec4 &color);
  private:
	  static GLint colorLoc;
  };
};
