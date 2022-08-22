#pragma once
// 2017

#include <glm/glm.hpp>

#include "widget.h"
#include "value.h"
#include "program.h"
#include "uniformWriter.h"

/**

\class IVecSliderWidget

Clase para seleccionar el valor de un vector de enteros.

*/

namespace PGUPV {
	/**
	Template que representa un varios sliders combinados para dar valores a un tipo compuesto de
	enteros, como por ejemplo ivec3, ivec4, etc.

	\param V tipo de la variable a actualizar (glm::ivec4, glm::ivec2...)
	*/
	template <typename V>
	class IVecSliderWidget : public Widget {
	public:
		/**
		Constructor para un widget que representa una vector de tipo V (p.e., glm::ivec2, glm::ivec4, etc.)
		\param label Etiqueta del control
		\param value Valor inicial
		\param limits Rango de cada componente. Dado como una lista de valores: minx, maxx, miny, maxy, minz, maxz, minw, maxw
		(sólo hay que proporcionar tantos rangos como componentes tenga el vector)
		\param labels Si está presente, es la etiqueta que se mostrará para cada componente
		*/
		IVecSliderWidget(const std::string &label, const V &value, typename V::value_type min, typename V::value_type max) :
			value(value), min(min), max(max), displayFormat("%.0f") {
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
		IVecSliderWidget(const std::string &label, const V &value, typename V::value_type min, typename V::value_type max,
			std::shared_ptr<Program> program, const std::string &uniform) :
			IVecSliderWidget(label, value, min, max) {
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
		void set(const V &val, bool notifyListeners = true) {
			this->value.setValue(val, notifyListeners);
		};

		/**
		Devuelve un objeto Value<V>, al que se le puede asociar, por ejemplo un listener
		\see PGUPV::Value
		*/
		Value<V>& getValue() { return value; };
		typename V::value_type getMin() const { return min; };
		typename V::value_type getMax() const { return max; };

		void renderWidget() override {
			auto v = value.getValue();
			if (renderSliderInt(v))
				value.setValue(v);
		}

	protected:
		Value<V> value;
		typename V::value_type min, max;
		std::string displayFormat;
	private:
		bool renderSliderInt(V &value);
	};

	typedef IVecSliderWidget<glm::ivec2> IVec2SliderWidget;
	typedef IVecSliderWidget<glm::ivec3> IVec3SliderWidget;
	typedef IVecSliderWidget<glm::ivec4> IVec4SliderWidget;
};
