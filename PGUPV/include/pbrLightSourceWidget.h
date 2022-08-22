#pragma once
// 2017

#include "uboPBRLightSources.h"
#include "common.h"
#include "camera.h"

namespace PGUPV {
  class PBRLightSourceWidget : public Widget {
  public:
      PBRLightSourceWidget(std::shared_ptr<UBOPBRLightSources> lightSources, float positionRange,
      std::shared_ptr<const Camera> camera = std::shared_ptr<const Camera>()) :
      lightSources(lightSources), positionRange(positionRange), cameraPtr(camera) {
      build();
    };


    void renderWidget() override {
      selectedLightSource->render();
      lightEnabled->render();

      separator.render();
      sourceColorLabel.render();

      colorSelector->render();
      intensitySelector->render();

      separator.render();

      posDirLabel.render();
      isDirectional->render();
      dirSelector->render();
      posSelector->render();

      if (!isDirectional->get()) {
        // If it is directional, the attenuation is not used

        attenuation->render();

        // and the spotlight parameters are not used either
        separator.render();

        spotLightLabel.render();
        spotAngle->render();
        spotDirSelector->render();
        spotExponentSelector->render();
      }
    }
  protected:
    std::vector<std::shared_ptr<Widget>> widgets;
    std::shared_ptr<UBOPBRLightSources> lightSources;
    float positionRange;
    std::shared_ptr<const Camera> cameraPtr;

    std::shared_ptr<ListBoxWidget<>> selectedLightSource;
    std::shared_ptr<CheckBoxWidget> lightEnabled, isDirectional;
    std::shared_ptr<RGBColorWidget> colorSelector;
    std::shared_ptr<FloatSliderWidget> intensitySelector;
    Separator separator;
    Label sourceColorLabel, posDirLabel, spotLightLabel;
    std::shared_ptr<DirectionWidget> dirSelector, spotDirSelector;
    std::shared_ptr<Vec3SliderWidget> posSelector, attenuation;
    std::shared_ptr<FloatSliderWidget> spotAngle;
    std::shared_ptr<IntSliderWidget> spotExponentSelector;

    void build() {
      std::vector<std::string> lightSourceIdLabels;
      for (uint i = 0; i < lightSources->size(); i++) {
        lightSourceIdLabels.push_back(std::to_string(i));
      }
      selectedLightSource = std::make_shared<ListBoxWidget<>>("Fuente", lightSourceIdLabels);

      auto intWidgetUpdater = [this](int /*value*/) { updateWidget(); };

      selectedLightSource->getValue().addListener(intWidgetUpdater);

      lightEnabled = std::make_shared<CheckBoxWidget>("Encendida", lightSources->isOn(selectedLightSource->getSelected()));

      auto boolUBOUpdater = [this](bool ) { updateUBO(); };
      lightEnabled->getValue().addListener(boolUBOUpdater);

      sourceColorLabel.setText("Color de la fuente");

      colorSelector = std::make_shared<RGBColorWidget>("Color", glm::vec3(0.8f));
      colorSelector->getValue().addListener([this](const glm::vec3&) { updateUBO(); });
      intensitySelector = std::make_shared<FloatSliderWidget>("Intensidad", 1.0f, 0.1f, 200.f);
      intensitySelector->getValue().addListener([this](const float&) { updateUBO(); });

      posDirLabel.setText("Posición / Dirección");
      isDirectional = std::make_shared<CheckBoxWidget>("Direccional", false);
      isDirectional->getValue().addListener(
        [this](bool directional) {
        if (directional) {
          dirSelector->set(posSelector->get());
        }
        else {
          posSelector->set(dirSelector->get());
        }
        dirSelector->setVisible(directional);
        posSelector->setVisible(!directional);
        updateUBO();
      });

      posSelector = std::make_shared<Vec3SliderWidget>("Posición", glm::vec3(0.0f), -positionRange / 2.0f, positionRange / 2.f);

      auto vec3UBOUpdater = [this](glm::vec3 ) { updateUBO(); };

      posSelector->getValue().addListener(vec3UBOUpdater);

      dirSelector = std::make_shared<DirectionWidget>("Dirección", glm::vec3(0.0f));
      dirSelector->setCamera(cameraPtr);
      dirSelector->setVisible(false);
      dirSelector->getValue().addListener(vec3UBOUpdater);

      attenuation = std::make_shared<Vec3SliderWidget>("Atenuación", glm::vec3(0.0f), 0.f, 5.f);
      attenuation->getValue().addListener(vec3UBOUpdater);

      //// Cosas del foco

      spotLightLabel.setText("Foco");
      // Ángulo
      spotAngle = std::make_shared<FloatSliderWidget>("Ángulo", 30.0f, 0.0f, 180.0f);
      spotAngle->getValue().addListener([this](float ) { updateUBO(); });

      // Dirección del foco
      spotDirSelector = std::make_shared<DirectionWidget>("Dirección", glm::vec3(0.0f));
      spotDirSelector->setCamera(cameraPtr);
      spotDirSelector->getValue().addListener(vec3UBOUpdater);

      // Exponente
      spotExponentSelector = std::make_shared<IntSliderWidget>("Exponente", 30, 0, 255);
      spotExponentSelector->getValue().addListener([this](int ) { updateUBO(); });

      updateWidget();

    }

    void updateWidget() {
      PBRLightSourceParameters lsp = lightSources->getLightSource(selectedLightSource->getSelected());
      lightEnabled->set(lsp.enabled != 0);
      colorSelector->setColor(lsp.color);
      intensitySelector->set(lsp.intensity);

      dirSelector->set(glm::vec3(lsp.positionWorld));
      posSelector->set(glm::vec3(lsp.positionWorld));

      isDirectional->set(lsp.directional != 0);

      spotDirSelector->set(lsp.spotDirectionWorld);
      spotAngle->set(lsp.spotCutoff);
      spotExponentSelector->set((int)lsp.spotExponent);
      attenuation->set(lsp.attenuation);
    }

    void updateUBO() {
      PBRLightSourceParameters lsp = lightSources->getLightSource(selectedLightSource->getSelected());
      lsp.enabled = lightEnabled->get();

      lsp.color = colorSelector->getColor();
      lsp.intensity = intensitySelector->get();
      lsp.scaledColor = lsp.color * lsp.intensity;

      if (isDirectional->get()) {
        lsp.positionWorld = glm::vec4(dirSelector->get(), 0.0f);
        lsp.directional = 1;
      }
      else {
        lsp.positionWorld = glm::vec4(posSelector->get(), 1.0f);
        lsp.directional = 0;
      }

      lsp.spotDirectionWorld = spotDirSelector->get();
      lsp.spotCutoff = spotAngle->get();
      lsp.spotExponent = (float)spotExponentSelector->get();
      lsp.spotCosCutoff = (float)cos(lsp.spotCutoff * M_PI / 180.0);
      lsp.attenuation = attenuation->get();
      lightSources->setLightSource(selectedLightSource->getSelected(), lsp);
    }
  };
}; // namespace PGUPV

