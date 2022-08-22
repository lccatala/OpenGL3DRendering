
#include <iostream>
#include "events.h"
#include "app.h"
#include "utils.h"
#include "log.h"

#include "json.hpp"

using PGUPV::Event;
using PGUPV::EventType;

using json = nlohmann::json;

std::ostream& PGUPV::operator<<(std::ostream& os, const Event& e) {
  json j;
  j["frame"] = App::getInstance().getCurrentFrame();
  j["type"] = PGUPV::to_underlying(e.type);

  switch (e.type) {
  case EventType::KeyboardEvent:
    j["key"] = PGUPV::to_underlying(e.keyboard.key);
    j["mod"] = e.keyboard.mod;
    j["state"] = PGUPV::to_underlying(e.keyboard.state);
    j["window"] = App::getInstance().getWindowIndex(e.keyboard.wsrc);
    break;
  case EventType::MouseButtonEvent:
    j["button"] = (uint)e.mouseButton.button;
    j["state"] = PGUPV::to_underlying(e.mouseButton.state);
    j["x"] = e.mouseButton.x;
    j["y"] = e.mouseButton.y;
    j["window"] = App::getInstance().getWindowIndex(e.mouseButton.wsrc);
    break;
  case EventType::MouseMotionEvent:
    j["state"] = e.mouseMotion.state;
    j["x"] = e.mouseMotion.x;
    j["y"] = e.mouseMotion.y;
    j["xrel"] = e.mouseMotion.xrel;
    j["yrel"] = e.mouseMotion.yrel;
    j["window"] = App::getInstance().getWindowIndex(e.mouseMotion.wsrc);
    break;
  case EventType::MouseWheelEvent:
    j["x"] = e.mouseWheel.x;
    j["y"] = e.mouseWheel.y;
    j["window"] = App::getInstance().getWindowIndex(e.mouseWheel.wsrc);
    break;
  case EventType::JoystickMotionEvent:
    j["axis"] = e.joystickMotion.axis;
    j["joystickId"] = e.joystickMotion.joystickId;
    j["value"] = e.joystickMotion.value;
    break;
  case EventType::JoystickHatMotionEvent:
    j["hatID"] = e.joystickHatMotion.hatId;
    j["joystickId"] = e.joystickHatMotion.joystickId;
    j["position"] = PGUPV::to_underlying(e.joystickHatMotion.position);
    break;
  case EventType::JoystickButtonEvent:
    j["button"] = e.joystickButton.button;
    j["joystickId"] = e.joystickButton.joystickId;
    j["state"] = PGUPV::to_underlying(e.joystickButton.state);
    break;
  case EventType::WindowResizedEvent:
    j["width"] = e.windowResized.width;
    j["height"] = e.windowResized.height;
    j["window"] = App::getInstance().getWindowIndex(e.windowResized.wsrc);
    break;
  case EventType::QuitEvent:
    break;
  case EventType::TextInputEvent:
    j["text"] = e.textInput.text;
    j["window"] = App::getInstance().getWindowIndex(e.textInput.wsrc);
    break;
  default:
    ERRT("Tipo de evento desconocido");
  }
  os << j.dump() << std::endl;

  return os;
}

bool PGUPV::readNextEventForFrame(std::istream & is, long frame, Event & result)
{
  static json j;

  std::string str;
  if (j.empty()) {
    try
    {
      getline(is, str);
      if (is.eof()) return false;
      j = json::parse(str);
    }
    catch (const std::exception&e)
    {
      ERRT(std::string("Error leyendo el fichero de eventos: ") + e.what());
    }
  }

  if (j["frame"].get<long>() != frame)
    return false;

  result.type = PGUPV::to_enum<EventType>(j["type"].get<int>());
  switch (result.type) {
  case EventType::KeyboardEvent:
    result.keyboard.key = PGUPV::to_enum<PGUPV::KeyCode>(j["key"].get<int>());
    result.keyboard.mod = j["mod"].get<ushort>();
    result.keyboard.state = PGUPV::to_enum<PGUPV::ButtonState>(j["state"].get<int>());
    result.keyboard.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  case EventType::MouseMotionEvent:
    result.mouseMotion.state = j["state"];
    result.mouseMotion.x = j["x"];
    result.mouseMotion.y = j["y"];
    result.mouseMotion.xrel = j["xrel"];
    result.mouseMotion.yrel = j["yrel"];
    result.keyboard.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  case EventType::MouseButtonEvent:
    result.mouseButton.button = static_cast<uint8_t>(j["button"].get<uint>());
    result.mouseButton.state = PGUPV::to_enum<PGUPV::ButtonState>(j["state"].get<int>());
    result.mouseButton.x = j["x"];
    result.mouseButton.y = j["y"];
    result.mouseButton.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  case EventType::MouseWheelEvent:
    result.mouseWheel.x = j["x"];
    result.mouseWheel.y = j["y"];
    result.mouseWheel.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  case EventType::JoystickMotionEvent:
    result.joystickMotion.axis = j["axis"];
    result.joystickMotion.joystickId = j["joystickId"];
    result.joystickMotion.value = j["value"];
    break;
  case EventType::JoystickHatMotionEvent:
    result.joystickHatMotion.hatId = j["hatID"];
    result.joystickHatMotion.joystickId = j["joystickId"];
    result.joystickHatMotion.position = PGUPV::to_enum<PGUPV::HatPosition>(j["position"].get<int>());
    break;
  case EventType::JoystickButtonEvent:
    result.joystickButton.button = j["button"];
    result.joystickButton.joystickId = j["joystickId"];
    result.joystickButton.state = PGUPV::to_enum<ButtonState>(j["state"].get<int>());
    break;
  case EventType::WindowResizedEvent:
    result.windowResized.width = j["width"];
    result.windowResized.height = j["height"];
    result.windowResized.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  case EventType::QuitEvent:
    break;
  case EventType::TextInputEvent:
  {
    std::string t = j["text"];
    PGUPV::strcpy_s(result.textInput.text, sizeof(result.textInput.text), t.c_str());
    result.textInput.wsrc = App::getInstance().getWindowPtr(j["window"].get<uint>());
    break;
  }
  default:
    ERRT("No se ha podido reconocer el evento " + j.dump());
  }

  j.clear();
  return true;
}

bool PGUPV::isFunctionKey(KeyCode code) {
  int icode = static_cast<int>(code);
  return (icode >= static_cast<int>(KeyCode::F1) &&
    icode <= static_cast<int>(KeyCode::F12));    
}