#include "app.h"
#include "log.h"

#include "eventSource.h"
#include "events.h"

using PGUPV::App;

bool PGUPV::LoadEventSource::getEvent(Event & e)
{
  return PGUPV::readNextEventForFrame(input, App::getInstance().getCurrentFrame(), e);
}

PGUPV::LoadFileEventSource::LoadFileEventSource(const std::string & path)
  : LoadEventSource(file)
{
  file.open(path);
  if (!file)
    ERRT("No se ha encontrado el fichero " + path);
}
