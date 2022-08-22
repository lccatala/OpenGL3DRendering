#pragma once

#include <iostream>
#include <functional>
#include <list>

namespace PGUPV {

	template <typename T>
	class Observable {
	public:
		typedef typename std::list<std::function<void(const T&)>>::iterator SubscriptionId;
		/**
		Registra un oyente.
		\return Un identificador para poder cancelar la subscripción más adelante, usando removeListener
		*/
		SubscriptionId addListener(std::function<void(const T&)> listener) {
			listeners.push_back(listener);
			return std::prev(listeners.end());
		}

		void removeListener(SubscriptionId idListener) {
			if (!listeners.empty())
				listeners.erase(idListener);
		}

		void notify(const T& value) {
			for (auto l : listeners) {
				l(value);
			}
		};
	private:
		std::list <std::function<void(const T&)>> listeners;
	};

};
