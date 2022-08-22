#pragma once
// 2020

#include <vector>
#include <string>

namespace PGUPV {
	class CommonDialogs {
	public:
		enum class DialogResult {
			OK, CANCEL, ERROR
		};

		/**
		Muestra el diálogo estándar para seleccionar un directorio
		\param initialPath el directorio inicial (se puede dejar vacío, o pasar una ruta absoluta)
		\param result el directorio seleccionado por el usuario si la función devuelve OK, o un mensaje
		de error si devuelve ERROR
		\return OK si el usuario pulsa Aceptar, CANCEL si cancela o cierra el diálogo, o ERROR si se produce un error
		*/
		static DialogResult SelectFolder(const std::string &initialPath, std::string &result);

		/**
		Muestra el diálogo estándar para seleccionar un fichero
		\param filterList un vector de cadenas con las extensiones buscadas (p.e., {"png,gif;bmp"})
		\param initialPath el directorio inicial (se puede dejar vacío, o pasar una ruta absoluta)
		\param result la ruta del fichero seleccionado por el usuario si la función devuelve OK, o un mensaje
		de error si devuelve ERROR
		\return OK si el usuario pulsa Aceptar, CANCEL si cancela o cierra el diálogo, o ERROR si se produce un error
		*/
		static DialogResult OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result);

		/**
		Muestra el diálogo estándar para seleccionar uno o más ficheros
		\param filterList un vector de cadenas con las extensiones buscadas (p.e., {"png,gif;bmp"})
		\param initialPath el directorio inicial (se puede dejar vacío, o pasar una ruta absoluta)
		\param result la ruta de los ficheros seleccionados por el usuario si la función devuelve OK, o un mensaje
		de error si devuelve ERROR
		\return OK si el usuario pulsa Aceptar, CANCEL si cancela o cierra el diálogo, o ERROR si se produce un error
		*/
		static DialogResult OpenFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::vector<std::string> &result);

		/**
		Muestra el diálogo estándar para seleccionar un fichero para guardar
		\param filterList un vector de cadenas con las extensiones buscadas (p.e., {"png,gif;bmp"})
		\param initialPath el directorio inicial (se puede dejar vacío, o pasar una ruta absoluta)
		\param result la ruta del fichero seleccionado por el usuario si la función devuelve OK, o un mensaje
		de error si devuelve ERROR
		\return OK si el usuario pulsa Aceptar, CANCEL si cancela o cierra el diálogo, o ERROR si se produce un error
		*/
		static DialogResult SaveFile(const std::vector<std::string> &filterList, const std::string &initialPath, std::string &result);
	};
}
