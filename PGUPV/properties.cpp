#include <fstream>

#include "log.h"
#include "properties.h"
#include "utils.h"

using PGUPV::Properties;
using std::string;

Properties::Properties() {
	reset();
}

// Divide la cadena en dos: propiedad y valor, que están separados por un =
bool split(const string &input, string &prop, string &val) {
	std::string i(input);
	PGUPV::trim(i);

	auto eq = i.find('=');
	if (eq == i.npos) return false;

	std::string p = i.substr(0, eq);
	PGUPV::trim(p);
	prop = p;


	std::string rhs = i.substr(eq + 1);
	PGUPV::trim(rhs);

	// Puede haber valores vacios
	val = rhs;
	return true;
}


bool isComment(const string &line) {
	return PGUPV::starts_with(line, "#");
}

bool isSection(const string &line, string &section) {
	string::size_type i, j, len = line.size();

	i = 0;
	while (i != len && isspace(line[i])) i++;

	if (i == len || line[i] != '[') return false;

	// i: primer carácter del nombre de la sección
	i++;
	while (i != len && isspace(line[i])) i++;
	if (i == len) return false;

	// j: primer carácter que no pertenece al nombre de la sección
	j = i + 1;
	while (j != len && !isspace(line[j]) && line[j] != ']') j++;

	if (j == len) throw std::invalid_argument("Sección mal construida");

	// Comprobar que hay un ']'
	string::size_type k = j;
	while (k != len && line[k] != ']') k++;

	if (k == len) throw std::invalid_argument("Sección mal construida");

	section = line.substr(i, j - i);
	return true;
}

bool Properties::load(const std::string &fname) {
	reset();
	std::ifstream f(fname.c_str());

	if (!f) return false;
	string line, sect = "";
	while (std::getline(f, line)) {
		try {
			if (isComment(line)) continue;
			if (isSection(line, sect)) continue;
			string p, v;
			if (split(line, p, v)) {
				if (sect != "") pm.insert(std::pair<string, string>(sect + "." + p, v));
				else pm.insert(std::pair<string, string>(p, v));
			}
		}
		catch (std::invalid_argument &e) {
			ERR(std::string("Error leyendo el fichero \"") + fname + " " + e.what());
			f.close();
			reset();
			return false;
		}
	}
	this->filename = fname;
	f.close();
	dirty = false;
	return true;
}


bool Properties::save(void) {
	if (!dirty) return true;
	return saveAs(filename);
}


bool Properties::saveAs(const std::string &fname) {

	if (!dirty && fname == this->filename) return true;

	std::ofstream f(fname.c_str());

	if (!f) return false;

	// Primero, escribir todas las propiedades sin sección
	for (auto i = pm.cbegin(); i != pm.end(); i++)
		if (i->first.find('.') == string::npos)
			f << i->first << " = " << i->second << std::endl;

	// Luego, las que están en alguna sección
	string current_section;
	for (auto i = pm.begin(); i != pm.end(); i++) {
		string::size_type p = i->first.find('.');
		if (p == string::npos) continue;
		string section = i->first.substr(0, p);
		if (section != current_section) {
			f << std::endl << "[" << section << "]" << std::endl;
			current_section = section;
		}

		f << i->first.substr(p + 1, i->first.length() - p - 1);
		f << " = " << i->second << std::endl;
	}
	f.close();
	dirty = false;
	this->filename = fname;
	return true;
}


// Vuelve a cargar el fichero de propiedades
bool Properties::refresh() {
	return load(filename);
}


// Devuelve true si encuentra el valor de la propiedad, copiándolo en value
bool Properties::value(const string &prop, std::string &v) {
	auto i = pm.find(prop);

	if (i == pm.end())
		return false;
	v = i->second;
	return true;
}

bool Properties::value(const std::string &prop, bool &v) {
	std::string tmp;
	if (!value(prop, tmp))
		return false;
	tmp = PGUPV::to_lower(tmp);
	v = (tmp == "true" || tmp == "1");
	return true;
}

bool Properties::value(const std::string &prop, int &v) {
	std::string tmp;
	if (!value(prop, tmp)) 
		return false;
	v = std::stoi(tmp);
	return true;
}

bool Properties::value(const std::string &prop, unsigned int &v) {
	std::string tmp;
	if (!value(prop, tmp))
		return false;
	v = std::stoul(tmp);
	return true;
}

void Properties::reset(void) {
	filename = "";
	dirty = false;
	pm.clear();
}


// Actualiza el valor o añade la propiedad
void Properties::setValue(const std::string &prop, const std::string &value) {
#ifdef _DEBUG
	if (prop.find(' ') != prop.npos) {
		throw std::invalid_argument("En nombre de las propiedades no puede tener espacios");
	}
#endif
	auto i = pm.find(prop);

	auto v = value;
	PGUPV::trim(v);
	if (i == pm.end()) {
		pm.insert(std::pair<string, string>(prop, v));
		dirty = true;
	}
	else {
		if (i->second != v) {
			i->second = v;
			dirty = true;
		}
	}
}

bool Properties::remove(const std::string & prop)
{
	bool res = pm.erase(prop) > 0;
	if (res) dirty = true;
	return res;
}

const std::string &Properties::getKey(size_t n) {
	size_t i = 0;
	decltype(pm)::const_iterator iter;
	for (iter = pm.cbegin(); iter != pm.cend() && i != n; ++iter, ++i)
		;
	if (iter == pm.cend())
		ERRT("La propiedad " + std::to_string(n) + " no existe");
	return iter->first;
}
