#include "vecSliderWidget.h"

#include <guipg.h>

using PGUPV::VecSliderWidget;

template <typename V>
bool VecSliderWidget<V>::renderSliderWidget(V &v) {
	return GUILib::SliderFloat(label, v, min, max, displayFormat.c_str());
}

template class VecSliderWidget<glm::vec2>;
template class VecSliderWidget<glm::vec3>;
template class VecSliderWidget<glm::vec4>;

