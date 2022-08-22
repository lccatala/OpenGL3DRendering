#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm/vec2.hpp>

#include "value.h"
#include "widget.h"

/**
\class Panel

Un panel es un contenedor de widgets que se puede mover en la ventana.

*/

namespace PGUPV {
	class Panel : public std::enable_shared_from_this<Panel> {
	public:
		~Panel();
		void addWidget(std::shared_ptr<Widget> widget);
		std::string getName() const;
		void setPosition(float x, float y);
		void setSize(float width, float height);
		void render();

		Value<bool> &getVisibleValue();
		void setVisible(bool panelVisible = true);
		bool isVisible() const;

	protected:
		explicit Panel(const std::string &name);
		std::string name;
		glm::vec2 pos, size;
		Value<bool> visible;
		std::vector<std::shared_ptr<Widget>> children;

		// BaseRenderer and Window are the only classes who can create Panels
		friend class BaseRenderer;
		friend class Window;
	};
}; // namespace
