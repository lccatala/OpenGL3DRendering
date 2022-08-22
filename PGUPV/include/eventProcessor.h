#pragma once

#include <fstream>
#include <string>  // for string

namespace PGUPV {

  union Event;

  class EventProcessor {
  public:
    virtual ~EventProcessor() = default;
    virtual void dispatchPendingEvents() = 0;
  };

  class App;

  class AppEventProcessor : public EventProcessor {
  public:
    AppEventProcessor(App &app) :
      app(app) {};
    void dispatchPendingEvents() override;
  protected:
    void processEvent(const Event &e);
    App &app;
  };


  class SaveToStreamAndDispatchEventProcessor : public AppEventProcessor {
  public:
    SaveToStreamAndDispatchEventProcessor(App &app,
      std::ostream &output) :
      AppEventProcessor(app), output(output) {};
    void dispatchPendingEvents() override;
  private:
    std::ostream &output;
  };

  class SaveToFileAndDispatchEventProcessor : public SaveToStreamAndDispatchEventProcessor {
  public:
    SaveToFileAndDispatchEventProcessor(App &app, const std::string &filename);
  private:
    std::ofstream file;
  };
};