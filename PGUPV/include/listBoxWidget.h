
#pragma once


#include <glm/glm.hpp>

#include "widget.h"
#include "value.h"
#include "program.h"
#include "log.h"

/**

\class ListBoxWidget

Clase para seleccionar un elemento de una lista.  Ejemplo de uso:
*/

namespace PGUPV {

  bool renderListBox(const std::string & label, int *i, const char **ptr, size_t nelems, int numItems);


  template<typename T = void *>
  class ListBoxWidget : public Widget {
  public:
    /**
     Constructor con los elementos de la lista
     \param label Título de la lista
     \param elements Lista de cadenas con los elementos de la lista
     \param selected índice del elemento seleccionado inicialmente (el primero por defecto)
     */
    ListBoxWidget(const std::string &label, const std::vector<std::string> &elements, const int selected = 0)
      : value(selected), showNumItems(-1) {
      LIBINFO("ListBoxWidget created: " + getName());
      setLabel(label);
      setElements(elements, false);
	  setSelected(selected, false);
    }
    /**
     Constructor con los elementos de la lista
     \param label Título de la lista
     \param elements Lista de cadenas con los elementos de la lista
     \param selected índice del elemento seleccionado inicialmente
     \param program referencia al programa que contiene el uniform a modificar
     \param uniform nombre del uniform a modificar
     */
    ListBoxWidget(const std::string &label, const std::vector<std::string> &elements, const int selected,
      std::shared_ptr<Program> program, const std::string &uniform) : ListBoxWidget(label, elements, selected) {
      this->program = program;
      GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
      auto writeUniform = [ul, program](int q) {
        program->use();
        glUniform1i(ul, q);
      };
      writeUniform(selected);
      this->value.addListener(writeUniform);
    }

    /**
    \return el índice del elemento seleccionado
    */
    int getSelected() const {
      return value.getValue();
    }

	const std::string &getSelectedElement() const {
		return elements[value.getValue()];
	}

    /**
    \return el valor que especificó el usuario asociado al elemento seleccionado
    */
    T getSelectedUserData() const {
      return userData[value.getValue()];
    }
    /**
    Establece el elemento seleccionado
    \param idx índice del elemento a seleccionar
    */
    void setSelected(const int idx, bool notifyListeners = true) {
      value.setValue(idx, notifyListeners);
    }

	/**
	Establece el elemento seleccionado, pero dado el valor del tipo alternativo
	\param val valor alternativo del elemento a seleccionar
	*/
	void setSelected(const T val, bool notifyListeners = true) {
		const auto it = std::find(userData.begin(), userData.end(), val);
		if (it == userData.end()) {
			WARN("Elemento no encontrado. Ignorando la petición de selección");
		}
		else {
			value.setValue(static_cast<int>(it - userData.begin()), notifyListeners);
		}
	}



    /**
    \return una referencia al objeto Value<int> que contiene la selección actual. Con esa
      referencia puedes instalar, por ejemplo, un listener para saber cuándo cambia la selección
    */
    Value<int>& getValue() { return value; };

    /**
    Establece el número de elementos que serán visibles en la lista
    \param elem El número de elementos que se muestran en la lista sin necesidad de hacer scroll
    */
    void setNumVisibleElements(int elem) { showNumItems = elem; }
    /**
    Modifica los elementos mostrados en la lista.
    \param elems lista de etiquetas a mostrar en el control
    \warning descarta los datos del usuario asociados a la lista, ya que podrían no coincidir
    con los nuevos elementos. Si quieres asociar un dato con cada elemento, vuelve a llamar
    a setUserData.
    */
    void setElements(const std::vector<std::string> &elems, bool notifyListeners = true) {
      elements = elems;
      userData.clear();
	  refreshList(notifyListeners);
    }


	void setElements(const std::vector<std::string> &elems, const std::vector<T> &userElemens, bool notifyListeners = true) {
		elements = elems;
		setUserData(userElemens);
		refreshList(notifyListeners);
	}



    /**
    Una listbox puede almacenar tanto la lista de etiquetas (visible para el usuario, que
    las utiliza para seleccionar) como una lista de punteros. Esta segunda lista tiene
    el mismo número de elementos que la primera, pero no es visible para el usuario.
    Se puede obtener el valor asociado a una posición con la llamada ListBoxWidget::getUserData
    \param userData vector de punteros genéricos, del mismo tamaño que el número de elementos de la lista
    */
    void setUserData(std::vector<T> data) {
      if (data.size() != elements.size())
        ERRT("El tamaño de los vectores debe ser igual");
      this->userData = data;
    }

    /**
    \return el valor de usuario asociado a la posición indicada
    */
    T getUserData(unsigned int id) const {
      if (id >= userData.size())
        ERRT("Ese elemento no existe");
      return userData[id];
    }

    void renderWidget() override {
      auto i = value.isEmpty() ? 0 : value.getValue();
      const char **ptr = elementsCString.empty() ? nullptr : &elementsCString[0];
      if (renderListBox(label, &i, ptr, elements.size(), showNumItems))
        value.setValue(i);
    }
  protected:
    Value<int> value;
    std::vector<std::string> elements;
    std::vector<T> userData;
    std::vector<const char *>elementsCString;
    int showNumItems;

	void refreshList(bool notifyListeners) {
		elementsCString.clear();
		if (!elements.empty()) {
			for (size_t i = 0; i < elements.size(); i++)
				elementsCString.push_back(elements[i].c_str());
			setSelected(0, notifyListeners);
		}
		else
			value.clear();
		// TODO: what if there are no elements? we should notify empty values!!!

	}
  };
};
