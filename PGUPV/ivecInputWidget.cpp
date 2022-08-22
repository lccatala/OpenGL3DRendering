#include <guipg.h>

#include "ivecInputWidget.h"

using PGUPV::IVecInputWidget;

template<typename V>
bool IVecInputWidget<V>::renderIVecInputWidget(const std::string & l, V & v)
{
	return GUILib::InputInt(l, v);
}

template class IVecInputWidget<glm::ivec2>;
template class IVecInputWidget<glm::ivec3>;
template class IVecInputWidget<glm::ivec4>;
