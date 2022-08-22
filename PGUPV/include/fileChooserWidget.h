#pragma once

#include "widget.h"
#include "label.h"
#include "button.h"
#include "listBoxWidget.h"

#include <vector>
#include <string>

namespace PGUPV {

	class FileChooserWidget : public Widget {
	public:
		FileChooserWidget();
		void renderWidget() override;
		/**
		Establece las extensiones de los ficheros a mostrar. Por ejemplo:
		fc.setFilter({"*.txt", "*.md"});
		\param extensions vector de cadenas con las extensiones a mostrar
		*/
		void setFilter(std::vector<std::string> extensions);
		/**
		Busca todos los ficheros con las extensiones indicadas a partir del
		directorio indicado, y en todos sus hijos (cuidado si hay muchos!)
		\param recursive si true, busca los ficheros también en los subdirectorios
		Por defecto, false (sólo muestra los ficheros del directorio seleccionado)
		*/
		void setRecursive(bool recursive);

		void setDirectory(const std::string &dir);

		/**
		\return una referencia al objeto Value<string> que contiene la selección actual. Con esa
		referencia puedes instalar, por ejemplo, un listener para saber cuándo cambia la selección
		*/
		Value<std::string> &getValue() {
			return value;
		}
	private:
		bool selectDir(std::string &newDir);
		void refresh();
		std::string directory;
		Label currentDirLabel;
		Button changeDirBtn;
		ListBoxWidget<std::string> filenames;
		std::vector<std::string> showOnlyThisExtensions;
		bool searchInSubfolders;
		bool mustRefresh;

		Value<std::string> value;
	};
};