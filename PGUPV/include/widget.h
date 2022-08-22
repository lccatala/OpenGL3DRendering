#pragma once

#include <memory>
#include <string>  // for string

namespace PGUPV {
  class Program;
  class Panel;

  class Widget {
  public:
    Widget() : visible(true), disabled(false), preferredWidth(-1.0f) {};
    virtual ~Widget() {};

    std::string getName() const { return name; };

    void setLabel(const std::string &widgetLabel) { label = widgetLabel; }
    std::string getLabel() const { return label; }

    void setParent(std::shared_ptr<Panel> parentOfWidget) {
      parent = parentOfWidget;
    }

    bool isVisible() const { return visible; }
    virtual void setVisible(bool visibleWidget = true) {
      visible = visibleWidget;
    }

	void render();
    virtual void renderWidget() = 0;

	void setDisabled(bool widgetDisabled = true) {
		disabled = widgetDisabled;
	}

	bool isDisabled() const {
		return disabled;
	}

	void setWidth(float pct);
  protected:
    bool visible, disabled;

    static unsigned long counter;

    std::string label, name;
    std::shared_ptr<Program> program;
    int uniformLoc;
    std::shared_ptr<Panel> parent;
	float preferredWidth;
  };
};