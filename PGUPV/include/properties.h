#pragma once
#include <string>
#include <map>

namespace PGUPV {
  //! Clase que se encarga de cargar un fichero de propiedades y acceder a las mismas.
  class Properties {
  public:
    Properties();
    //! Cargar un fichero de propiedades.
    bool load(const std::string &filename);
    //! Guardar las propiedades cargadas.
    bool save();
    //! Guarda las propiedades en un nuevo fichero.
    bool saveAs(const std::string &filename);
    //! Vuelve a cargar el fichero de propiedades.
    bool refresh();
    //! Devuelve true si encuentra el valor de la propiedad, copiándolo en value.
    /**
    \param &prop nombre de la propiedad.
    \param &value valor de la propiedad.
    \returns cierto si existe la propiedad. En 'value' se almacena su valor.
    */
    bool value(const std::string &prop, std::string &v);
	bool value(const std::string &prop, bool &v);
	bool value(const std::string &prop, int &v);
	bool value(const std::string &prop, unsigned int &v);

    //! Actualiza el valor o añade la propiedad.
    /**
    \param &prop nombre de la propiedad.
    \param &value nuevo valor para la propiedad.
    */
    void setValue(const std::string &prop, const std::string &value);
	/**
	Elimina la propiedad
	\param prop nombre de la propiedad
	\return si existía y se borró
	*/
	bool remove(const std::string &prop);
	//! Devuelve el número de propiedades almacenadas
	size_t numProperties() const { return pm.size(); }

	//! Devuelve la clave la propiedad n-ésima
	const std::string &getKey(size_t n);
  protected:
    //! Borra las propiedades cargadas.
    void reset(void);
    //! Caché de propiedades.
    std::map<std::string, std::string> pm;
    //! Nombre del fichero de propiedades cargado.
    std::string filename;
    //! ¿Se ha modificado desde que se cargó?
    bool dirty;
  };

};
