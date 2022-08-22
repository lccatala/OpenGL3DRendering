
#pragma once
// 2017

#include <functional>
#include <glm/vec3.hpp>

#include "widget.h"
#include "value.h"


/**

\class directionWidget

Clase para seleccionar una dirección.

*/

namespace PGUPV {
	class Program;
  class Camera;

	class DirectionWidget : public Widget {
	public:
		DirectionWidget(const std::string &label, const glm::vec3 &value = glm::vec3(1.0f, 0.0f, 0.0f));
		DirectionWidget(const std::string &label, const glm::vec3 &value,
			std::shared_ptr<Program> program, const std::string &uniform);
    void setCamera(std::shared_ptr<const Camera> camera) {
      cameraPtr = camera;
    }
		glm::vec3 get() const;
		void set(const glm::vec3 &d, bool notifyListeners = true);
		Value<glm::vec3>& getValue();
		void renderWidget() override;
	protected:
		Value<glm::vec3> value;
    std::shared_ptr<const Camera> cameraPtr;
	};
};
