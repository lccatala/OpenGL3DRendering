#pragma once
// 2017


#include "widget.h"
#include "value.h"

namespace PGUPV {
	class Program;
	
	class FloatSliderWidget : public Widget {
	public:
		FloatSliderWidget(const std::string &label, float value, float v_min, float v_max);
		FloatSliderWidget(const std::string &label, float value, float min, float max,
			std::shared_ptr<Program> program, const std::string &uniform);
		void setDisplayFormat(const std::string &format);
		const std::string getDisplayFormat();
		/**
		\return Devuelve el valor actual del slider
		*/
		float get() const;

		/**
		Establece el slider al valor indicado
		\param v el nuevo valor del slider. Si est� fuera del rango, se ajustar� al m�ximo o al m�nimo
		*/
		void set(const float v, bool notifyListeners = true);
		Value<float>& getValue();

		float getMin() const;
		float getMax() const;

		void renderWidget() override;

	private:
		float min, max;
		std::string displayFormat;
		Value<float> value;
	};
}
