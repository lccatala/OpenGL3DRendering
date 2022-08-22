#pragma once
// 2019

#include "widget.h"

namespace PGUPV {
	class ProgressBar : public Widget {
	public:
		void setFraction(float f);
		void renderWidget() override;
	private:
		float fraction;
	};
}
