#pragma once
// 2017
#include "intWidget.h"

namespace PGUPV {
	class IntSliderWidget : public IntWidget {
	public:
		IntSliderWidget(const std::string &label, int value, int v_min, int v_max);
		IntSliderWidget(const std::string &label, int value, int min, int max,
			std::shared_ptr<Program> program, const std::string &uniform);
		void setDisplayFormat(const std::string &format);
		const std::string getDisplayFormat();
		void renderWidget() override;
	private:
		std::string displayFormat;
	};
}
