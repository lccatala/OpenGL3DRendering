#pragma once
// 2017

#include "widget.h"

namespace PGUPV {
	class Label : public Widget {
	public:
		Label() = default;
		Label(const std::string &text);
		void renderWidget() override;
		void setText(const std::string &t);
	};

}
