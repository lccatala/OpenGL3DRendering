#pragma once
// 2017

#include "widget.h"
#include "value.h"

/**

\class CheckBoxWidget

*/
namespace PGUPV {
	class Program;

	class CheckBoxWidget : public Widget {
	public:
		/**
		Constructor de un checkbox. Usa la llamada getValue() para obtener el valor actual o para instalar
		un listener.
		\param label etiqueta del control
		\param value valor inicial del control
		*/
		CheckBoxWidget(const std::string &label, const bool &value = false);
		/**
		Constructor de un checkbox.
		\param label etiqueta del control
		\param value valor inicial del control
		\param program programa que contiene el uniform a modificar
		\param uniform nombre de la variable (de tipo int) donde se escribirá el valor del control
		*/
		CheckBoxWidget(const std::string &label, const bool &value, std::shared_ptr<Program> program, const std::string &uniform);

		bool get() const;
		void set(const bool q, bool notifyListeners = true);

		Value<bool>& getValue();
		void renderWidget() override;
	protected:
		Value<bool> value;
	};
};
