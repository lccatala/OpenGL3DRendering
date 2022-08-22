#pragma once
// 2017


#include "widget.h"
#include "value.h"
#include "program.h"
#include "uniformWriter.h"

namespace PGUPV {
	template <typename V>
	class ColorWidget : public Widget {
	public:
		explicit ColorWidget(const std::string &label, const V &value = V(0.0f)) :
			value(value) {
			setLabel(label);
		}
		/**
		Constructor de un widget para seleccionar un color RGBA
		\param label etiqueta del control
		\param value color inicial
		\param program programa donde se encuentra el uniform a modificar
		\param uniform nombre de la variable uniform a modificar cada vez que el usuario interaccione con el control
		*/
		ColorWidget(const std::string &label, const V &value, std::shared_ptr<Program> program, const std::string &uniform) :
			ColorWidget<V>(label, value)
		{
			this->program = program;
			GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
			auto writeUniform = [ul, program](const V &q) {
				program->use();
				UniformWriter::write(ul, q);
			};
			writeUniform(value);
			this->value.addListener(writeUniform);
		}

		V getColor() const {
			return value.getValue();
		}
		void setColor(const V &c, bool notifyListeners = true) {
			value.setValue(c, notifyListeners);
		}

		Value<V>& getValue() { return value; };

		void renderWidget() override {
			auto c = value.getValue();
			if (renderColorWidget(label, c))
				value.setValue(c);
		}
	protected:
		Value<V> value;
	private:
		bool renderColorWidget(const std::string &label, V &color);
	};

	typedef ColorWidget<glm::vec3> RGBColorWidget;
	typedef ColorWidget<glm::vec4> RGBAColorWidget;
}
