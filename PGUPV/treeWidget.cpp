#include <guipg.h>

#include "treeWidget.h"

using PGUPV::TreeWidget;
using PGUPV::TreeNodeFlag;


bool PGUPV::_treeNodeDraw(void * data, TreeNodeFlags flags, const std::string & name)
{
	return GUILib::TreeNodeEx(data, flags, name);
}

bool PGUPV::_treeNodeIsItemClicked() {
	return GUILib::IsItemClicked();
}


void PGUPV::_treeNodeTreePop() {
	GUILib::TreePop();
}