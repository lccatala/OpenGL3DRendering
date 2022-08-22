#pragma once
// 2017

#include "widget.h"
#include "value.h"

namespace PGUPV {
	class IntWidget : public Widget {
	public:
		IntWidget(const std::string &label, int value, int v_min, int v_max);
		IntWidget(const std::string &label, int value, int min, int max,
			std::shared_ptr<Program> program, const std::string &uniform);

		/**
		\return Devuelve el valor actual del slider
		*/
		int get() const;

		/**
		Establece el slider al valor indicado
		\param v el nuevo valor del slider. Si está fuera del rango, se ajustará al máximo o al mínimo
		*/
		void set(const int v, bool notifyListeners = true);

		Value<int>& getValue();

		int getMin() const;
		int getMax() const;
		/**
		Establece el nuevo mínimo
		\param min Nuevo valor mínimo
		*/
		void setMin(int minimum, bool notifyListeners = true);

		/**
		Establece el nuevo máximo
		\param min Nuevo valor máximo
		*/
		void setMax(int maximum, bool notifyListeners = true);
	protected:
		int min, max;
		Value<int> value;
	};
}
