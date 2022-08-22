
#include <guipg.h>
#include "colorWidget.h"

using PGUPV::ColorWidget;

template <typename T> 
bool ColorWidget<T>::renderColorWidget(const std::string & l, T & color)
{
	return GUILib::ColorEdit(l, color);
}

template class ColorWidget<glm::vec3>;
template class ColorWidget<glm::vec4>;

