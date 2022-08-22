#pragma once
// 2017

#include <vector>
#include "widget.h"

namespace PGUPV {
	class HBox : public Widget {
	public:
		HBox() = default;
		void addChild(std::shared_ptr<Widget> child);
		void renderWidget() override;
	private:
		std::vector<std::shared_ptr<Widget>> children;
	};

};
