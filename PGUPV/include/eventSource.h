#pragma once

#include <istream>
#include <fstream>

namespace PGUPV {
  union Event;

  class EventSource {
  public:
    virtual ~EventSource() {};
    /**
    \return true si hay un evento, que lo devuelve en e. false si no había eventos pendientes
    */
    virtual bool getEvent(Event &e) = 0;
  };


  class NullEventSource : public EventSource {
  public:
    bool getEvent(Event &) override { return false; }
  };

  class LoadEventSource : public EventSource {
  public:
	  LoadEventSource(std::istream &input) : input(input) {};
	  bool getEvent(Event &e) override;
  protected:
	  std::istream &input;
  };

  class LoadFileEventSource : public LoadEventSource {
  public:
    LoadFileEventSource(const std::string &path);
  private:
    std::ifstream file;
  };
};