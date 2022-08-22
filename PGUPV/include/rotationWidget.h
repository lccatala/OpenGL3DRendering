#pragma once
// 2017

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "widget.h"
#include "value.h"

/**

\class RotationWidget

Clase para seleccionar una rotaci�n.

*/

namespace PGUPV {
	class Program;
	class RotationWidget : public Widget {
	public:
		RotationWidget(const std::string &label, const glm::mat4 &value = glm::mat4(1.0f));
		RotationWidget(const std::string &label, const glm::mat4 &value,
			std::shared_ptr<Program> program, const std::string &uniform);

		/**
		\return La rotaci�n en forma de cuaterni�n
		*/
		glm::quat getQuaternion() const;

		/**
		\return La rotaci�n en forma de matriz
		*/
		glm::mat4 get() const;

		/**
		Establece la rotaci�n
		\param q cuaterni�n que define la rotaci�n
		*/
		void setQuaternion(const glm::quat &q, bool notifyListeners = true);

		/**
		Establece la rotaci�n
		\param m matriz que define la rotaci�n
		*/
		void set(const glm::mat4 &m, bool notifyListeners = true);
		void set(const glm::mat3 &m, bool notifyListeners = true);

		Value<glm::quat>& getValue();

		void renderWidget() override;
	protected:
		Value<glm::quat> value;
	};
};
