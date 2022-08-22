#pragma once
// 2016

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "widget.h"
#include "vecSliderWidget.h"
#include "rotationWidget.h"
#include "label.h"


namespace PGUPV {
  /**
  \class TransformationWidget
  Control que representa las transformaciones de una matriz.
  Las transformaciones son del tipo Escalado, Rotación y Traslación,
  por ese orden y una de cada.
  */
  class TransformationWidget : public Widget {
  public:
    /**
    Constructor del widget. Recibe la matriz con la que se sincronizará (los controles del widget
    tomarán los valores de la matriz)
    \param label el título del control
    \param xform la matriz que da el valor inicial y la que se modificará al interactuar con
      el control
    */
    TransformationWidget(const std::string &label, const glm::mat4 &xform) : 
      translationRange(-10.0f, 10.0f), scaleRange(-5.0f, 5.0f), theMatrix(xform),
      updatingWidgets(false), updatingMatrix(false) {
      setLabel(label);
      build();
    }
    /**
    Cambia la matriz asociada al widget. Los controles cambiarán para mostrar el estado
    actual de la matriz.
    */
    void setMatrix(const glm::mat4 &xform) {
      WARN("--------------SETMATRIX (BEGIN)");
      theMatrix.setValue(xform);
      WARN("--------------SETMATRIX (END)");
    }

    void renderWidget() override {
      controlLabel.render();

      translation->render();
      rotation->render();
      scale->render();
    }

    Value<glm::mat4> &getValue() { return theMatrix; }

    void setTranslationRange(float min, float max) {
      translationRange = glm::vec2(min, max);
    }

    void setScaleRange(float min, float max) {
      scaleRange = glm::vec2(min, max);
    }
  private:


    void build() {

      controlLabel.setText(label);

      translation = std::make_shared<Vec3SliderWidget>("Translation", glm::vec3(0.0f), 0.0f, 1.0f);
      translation->getValue().addListener([this](const glm::vec3 &) { 
        WARN("========TRANS B");
        updateMatrix(); 
        WARN("========TRANS E");
      });

      rotation = std::make_shared<RotationWidget>("Rotation", glm::mat4(1.0f));
      rotation->getValue().addListener([this](const glm::quat &) { 
        WARN("========ROT B");
        updateMatrix();
        WARN("========ROT E");
      });


      scale = std::make_shared<Vec3SliderWidget>("Scale", glm::vec3(1.0f), -1.0f, 1.0f);
      scale->getValue().addListener([this](const glm::vec3 &s) { 
        WARN("========SCA B " + to_string(s));
        updateMatrix();
        WARN("========SCA E" + to_string(s));
      });

      updateWidgets();

      theMatrix.addListener([this](const glm::mat4 &) { updateWidgets(); });
    }

    

    void updateMatrix() {
      if (!updatingWidgets) {
        WARN("updateMatrix");
        updatingMatrix = true;
        glm::mat4 m = glm::translate(glm::mat4(1.0f), translation->get());
        m = m * rotation->get();
        m = glm::scale(m, scale->get());

        theMatrix.setValue(m);
        updatingMatrix = false;
      } else
        WARN("updateMatrix (ignored)");


    }

    void updateWidgets() {
      if (updatingMatrix)
        return;
      WARN("updateWidget");
      glm::mat4 rot;
      glm::vec3 trans, sca;
      if (!extractTRSfromMatrix(theMatrix.getValue(), trans, rot, sca))
        return;

      updatingWidgets = true;

      translation->set(trans);
      translation->setRange(translationRange.x, translationRange.y);

      rotation->set(rot);

      scale->set(sca);
      scale->setRange(scaleRange.x, scaleRange.y);

      updatingWidgets = false;
    }
    Label controlLabel;
    glm::vec2 translationRange, scaleRange;
    Value<glm::mat4> theMatrix;
    std::shared_ptr<RotationWidget> rotation;
    std::shared_ptr<Vec3SliderWidget> scale;
    std::shared_ptr<Vec3SliderWidget> translation;
    bool updatingWidgets, updatingMatrix;
  };
};
