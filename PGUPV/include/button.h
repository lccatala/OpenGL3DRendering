#pragma once
// 2017

#include "widget.h"
#include <functional>
#include <glm/vec2.hpp>

namespace PGUPV {
	class Button : public Widget {
	public:
		Button(const std::string &label, std::function<void()> callback);
		void renderWidget() override;
		void setSize(float width, float height);
	protected:
		std::function<void()> callback;
		glm::vec2 size;
	};
}
