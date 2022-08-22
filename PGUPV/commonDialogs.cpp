
#include "commonDialogs.h"
#include "utils.h"

#include <guipg.h>

using PGUPV::CommonDialogs;
using PGUPV::GUILib;

static CommonDialogs::DialogResult translate(GUILib::DialogResult r) {
	if (r == GUILib::DialogResult::OK)
		return CommonDialogs::DialogResult::OK;
	else if (r == GUILib::DialogResult::CANCEL)
		return CommonDialogs::DialogResult::CANCEL;
	else
		return CommonDialogs::DialogResult::ERROR;
}

std::string cleanInitialPath(const std::string &in) {
	std::string aux(in);
#ifdef _WIN32
	for (std::string::value_type &c : aux) {
		if (c == '/')
			c = '\\';
	}
#endif
	return PGUPV::removeDotAndDotDot(aux);
}

CommonDialogs::DialogResult CommonDialogs::SelectFolder(const std::string &initialPath, std::string &result) {

	return translate(GUILib::SelectFolder(cleanInitialPath(initialPath), result));
}

CommonDialogs::DialogResult CommonDialogs::OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result) {
	return translate(GUILib::OpenFile(filterList, cleanInitialPath(initialPath), result));
}

CommonDialogs::DialogResult CommonDialogs::OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::vector<std::string> &result) {
	return translate(GUILib::OpenFile(filterList, cleanInitialPath(initialPath), result));
}

CommonDialogs::DialogResult CommonDialogs::SaveFile(const std::vector<std::string>& filterList, const std::string & initialPath, std::string & result)
{
	return translate(GUILib::SaveFile(filterList, cleanInitialPath(initialPath), result));
}

