#pragma once
// 2017

#include "intWidget.h"

namespace PGUPV {
	class Program;

	class IntInputWidget : public IntWidget {
	public:
		IntInputWidget(const std::string &label, int value, int v_min, int v_max);
		IntInputWidget(const std::string &label, int value, int min, int max,
			std::shared_ptr<Program> program, const std::string &uniform);
		void renderWidget() override;
	};
}
