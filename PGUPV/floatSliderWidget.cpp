#include <guipg.h>

#include "program.h"
#include "uniformWriter.h"
#include "floatSliderWidget.h"

using PGUPV::FloatSliderWidget;
using PGUPV::Value;
using PGUPV::Program;

FloatSliderWidget::FloatSliderWidget(const std::string &label, float value, float v_min, float v_max) : 
    min(v_min), max(v_max), displayFormat("%.3f"), value(value)
{
    setLabel(label);
};
FloatSliderWidget::FloatSliderWidget(const std::string &label, float value, float min, float max,
                                     std::shared_ptr<Program> program, const std::string &uniform) : FloatSliderWidget(label, value, min, max)
{
    this->program = program;
    GLint ul = this->uniformLoc = program->getUniformLocation(uniform);
    auto writeUniform = [ul, program](float q) {
        auto prev = program->use();
        UniformWriter::write(ul, q);
        if (prev != nullptr)
            prev->use();
        else
            program->unUse();
    };
    writeUniform(value);
    this->value.addListener(writeUniform);
}

void FloatSliderWidget::setDisplayFormat(const std::string &format)
{
    displayFormat = format;
}

const std::string FloatSliderWidget::getDisplayFormat()
{
    return displayFormat;
}

void FloatSliderWidget::renderWidget() 
{
    float f = value.getValue();
    if (GUILib::SliderFloat(label, f, min, max, displayFormat))
        value.setValue(f);
}

float FloatSliderWidget::get() const
{
    return value.getValue();
}

void FloatSliderWidget::set(const float v, bool notifyListeners)
{
    if (v < min)
        value.setValue(min, notifyListeners);
    else if (v > max)
        value.setValue(max, notifyListeners);
    else
        value.setValue(v, notifyListeners);
}

Value<float> &FloatSliderWidget::getValue() { 
    return value; 
};

float FloatSliderWidget::getMin() const { 
    return min; 
};

float FloatSliderWidget::getMax() const { 
    return max; 
};

