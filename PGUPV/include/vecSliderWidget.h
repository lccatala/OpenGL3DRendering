#pragma once
// 2017


#include "widget.h"
#include "uniformWriter.h"
#include "value.h"
#include "program.h"

/**

\class VecSliderWidget

Clase para establecer el valor de las componentes de un vector.
Se puede hacer Ctrl+Clic para introducir un valor exacto por teclado.

*/

namespace PGUPV {
  /**
  Template que representa un varios sliders combinados para dar valores a un tipo compuesto, como
  por ejemplo vec3, ivec4, etc.

  \param V tipo de la variable a actualizar (glm::vec4, glm::ivec2...)
  */
  template <typename V>
  class VecSliderWidget : public Widget {
  public:
    /**
    Constructor para un widget que representa una vector de tipo V (p.e., glm::vec2, glm::ivec4, etc.)
    \param label Etiqueta del control
    \param value Valor inicial
    \param limits Rango de cada componente. Dado como una lista de valores: minx, maxx, miny, maxy, minz, maxz, minw, maxw
    (sólo hay que proporcionar tantos rangos como componentes tenga el vector)
    \param labels Si está presente, es la etiqueta que se mostrará para cada componente
    */
    VecSliderWidget(const std::string &label, const V &value, typename V::value_type min, typename V::value_type max) :
      value(value), min(min), max(max), displayFormat("%.3f"), power(1.0f) {
      setLabel(label);
    };


    /**
    Constructor para un widget que representa una vector de tipo V (p.e., glm::vec2, glm::ivec4, etc.)
    \param label Etiqueta del control
    \param value Valor inicial
    \param limits Rango de cada componente. Dado como una lista de valores: minx, maxx, miny, maxy, minz, maxz, minw, maxw
    (sólo hay que proporcionar tantos rangos como componentes tenga el vector)
    \param program Programa que contiene el uniform asociado
    \param uniform Nombre de la variable uniform de tipo V a actualizar cada vez que el usuario interaccione con el widget
    \param labels Si está presente, es la etiqueta que se mostrará para cada componente
    */
    VecSliderWidget(const std::string &label, const V &value, typename V::value_type min, typename V::value_type max,
      std::shared_ptr<Program> program, const std::string &uniform) :
      VecSliderWidget(label, value, min, max) {
      this->program = program;
      auto ul = this->uniformLoc = program->getUniformLocation(uniform);
      auto writeUniform = [ul, program](V q) {
        auto prev = program->use();
        UniformWriter::write(ul, q);
        if (prev != nullptr) prev->use();
        else program->unUse();
      };
      writeUniform(value);
      this->value.addListener(writeUniform);
    }

    //! El valor actual del widget
    V get() const {
      return value.getValue();
    }

    /**
    Establece el valor del widget
    */
    void set(const V &val, bool notifyListeners = true) { value.setValue(val, notifyListeners); };

    /**
    Devuelve un objeto Value<V>, al que se le puede asociar, por ejemplo un listener
    \see PGUPV::Value
    */
    Value<V>& getValue() { return value; };
    typename V::value_type getMin() const { return min; };
    typename V::value_type getMax() const { return max; };

    void setRange(typename V::value_type minimum, typename V::value_type maximum) {
      min = minimum;
      max = maximum;
    }

    void renderWidget() override {
      auto v = value.getValue();
      if (renderSliderWidget(v))
        value.setValue(v);
    }

  protected:
    Value<V> value;
    typename V::value_type min, max;
    std::string displayFormat;
    float power;

  private:
	  bool renderSliderWidget(V &v);
  };

  typedef VecSliderWidget<glm::vec2> Vec2SliderWidget;
  typedef VecSliderWidget<glm::vec3> Vec3SliderWidget;
  typedef VecSliderWidget<glm::vec4> Vec4SliderWidget;
};
