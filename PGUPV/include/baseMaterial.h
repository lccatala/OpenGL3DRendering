#pragma once

#include <string>

namespace PGUPV {
	class BaseMaterial {
	public:
		BaseMaterial() {};
		BaseMaterial(const std::string& matName) : name(matName) {};
		virtual ~BaseMaterial() = default;
		const std::string &getName() const {
			return name;
		}

		void setName(const std::string &matName) {
			name = matName;
		}

		//! Establece este material para usarlo en las siguientes llamadas de dibujo
		virtual void use() = 0;

		//! Desvincula las texturas que se asociaron con este material
		virtual void unuse() = 0;

	protected:
		std::string name;
	};
};
