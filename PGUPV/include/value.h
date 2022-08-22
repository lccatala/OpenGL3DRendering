#pragma once

#include "observable.h"
#include "log.h"

namespace PGUPV {
  template <typename T>
  class Value : public Observable<T> {
  public:
    Value() : 
      status(Status::Empty), other(nullptr)
    {};

    explicit Value(const T& value) : 
      status(Status::ValueDidntFire), value(value), other(nullptr)
    {};
    
    ~Value() {}


    /**
    \returns the stored value
    */
    T getValue() const {
      if (status == Status::Empty)
        ERRT("No hay valor");
      return value;
    };

    /**
    Sets the new value
    \warning It only notifies if there is a new value
    \param value The value
    */
    void setValue(const T& val, bool notifyListeners = true) {
      if (status == Status::ValueDidFire && this->value == val)
        return;
      value = val;
	  status = Status::ValueDidFire;
	  if (notifyListeners) {
		  this->notify(val);
	  }
    };

	void setEmpty(bool notifyListeners = true) {
		status = Status::Empty;
		if (notifyListeners) {

		}
	}
    /**
    Vincula el valor de esta variable al de otra
    \param other cada vez que la variable other cambie de valor, esta tomará
      dicho valor

    */
    void bind(Value<T> &otherValue) {
      unbind();
      setValue(otherValue.getValue());
      other = &otherValue;
      bound = otherValue.addListener([this](const T &value) { setValue(value); });
    }

    /**
    Desvincula esta variable 
    */
    void unbind() {
      if (other) {
        other->removeListener(bound);
        other = nullptr;
      }
    }

    void clear() {
      unbind();
      status = Status::Empty;
    }

    /**
    \return true si la variable no ha recibido ningún valor
    */
    bool isEmpty() const {
      return status == Status::Empty;
    }
  private:
    enum class Status {Empty, ValueDidntFire, ValueDidFire};
    Status status;
    T value;
    typename Observable<T>::SubscriptionId bound;
    Value<T> *other;
  };
};
