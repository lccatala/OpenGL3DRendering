#include "multiListBoxWidget.h"

#include <guipg.h>

bool PGUPV::_multilistboxBegin(const std::string& label) {
	return PGUPV::GUILib::MultiListBoxBegin(label);
}


bool PGUPV::_multilistboxItem(const std::string& element, bool& selected) {
	return PGUPV::GUILib::MultiListBoxItem(element, &selected);
}


void _multilistboxItem() {
	return PGUPV::GUILib::MultiListBoxEnd();
}
