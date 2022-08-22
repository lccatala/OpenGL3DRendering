#include <guipg.h>

#include "ivecSliderWidget.h"

using PGUPV::IVecSliderWidget;


template<typename V>
bool IVecSliderWidget<V>::renderSliderInt(V &v)
{
	return GUILib::SliderInt(label, v, min, max, displayFormat);
}

template class IVecSliderWidget<glm::ivec2>;
template class IVecSliderWidget<glm::ivec3>;
template class IVecSliderWidget<glm::ivec4>;

