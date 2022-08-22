#include "fileChooserWidget.h"
#include "hbox.h"
#include "utils.h"
#include "log.h"

#include <guipg.h>

using PGUPV::FileChooserWidget;

FileChooserWidget::FileChooserWidget() :
	changeDirBtn("...", [this]() {
		std::string newDir;
		if (selectDir(newDir)) {
			setDirectory(newDir);
		}
	}), 
	filenames("", {}), 
		showOnlyThisExtensions(std::vector<std::string>{ "*.*" }), 
		searchInSubfolders(false), 
		mustRefresh(true)
	{
	setDirectory(".");
	filenames.getValue().addListener([this](const int &) {
		value.setValue(filenames.getSelectedUserData());
	});
	filenames.getValue().clear();
	currentDirLabel.setWidth(85.0f);
	filenames.setWidth(1.0f);
	}

void FileChooserWidget::renderWidget() {
	
	currentDirLabel.render();
	GUILib::SameLine(GUILib::GetAvailableWidth() - 30.0f);
	changeDirBtn.render();
	if (mustRefresh) {
		refresh();
	}
	filenames.render();
}

void FileChooserWidget::setFilter(std::vector<std::string> extensions)
{
	showOnlyThisExtensions = extensions;
	mustRefresh = true;
}

void FileChooserWidget::setRecursive(bool recursive)
{
	searchInSubfolders = recursive;
	mustRefresh = true;
}

void FileChooserWidget::setDirectory(const std::string & dir)
{
	if (dir != directory) {
		directory = dir;
		currentDirLabel.setText(dir);
		mustRefresh = true;
	}
}

bool FileChooserWidget::selectDir(std::string & newDir)
{
	std::string result;
	auto initialDir = PGUPV::isAbsolutePath(directory) ? directory : PGUPV::relativeToAbsolute(directory);
	switch (GUILib::SelectFolder(initialDir, result)) {
	case GUILib::DialogResult::ERROR:
		ERR(result);
		break;
	case GUILib::DialogResult::CANCEL:
		break;
	case GUILib::DialogResult::OK:
		newDir = result;
		return true;
	}
	return false;
}

void FileChooserWidget::refresh()
{
	auto files = PGUPV::listFiles(directory, searchInSubfolders, showOnlyThisExtensions);

	std::vector<std::string> onlyNames;
	for (const auto &p : files) {
		onlyNames.push_back(getFilenameFromPath(p));
	}
	filenames.setElements(onlyNames, files, true);
	
	if (files.empty()) {
		value.notify("");
	}
	mustRefresh = false;
	filenames.setLabel(label);
}
