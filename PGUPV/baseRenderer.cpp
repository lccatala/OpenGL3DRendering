#include "baseRenderer.h"
#include "camerahandler.h"
#include "app.h"
#include "panel.h"          // for Panel
#include "window.h"         // for Window

using PGUPV::BaseRenderer;
using PGUPV::CameraHandler;
using PGUPV::Panel;



std::shared_ptr<CameraHandler> BaseRenderer::getCameraHandler() const {
  if (useInspectionCameraFlag)
    return inspectionCameraHandler;
  else
    return userCameraHandler;
};


void BaseRenderer::useInspectionCamera(bool use) {
  useInspectionCameraFlag = use;
  if (use && inspectionCameraHandler == nullptr) {
    inspectionCameraHandler = std::make_shared<OrbitCameraHandler>();
  }
  // update the projection matrix in case
  // the window was resized
  uint w = PGUPV::App::getInstance().getWindow().width();
  uint h = PGUPV::App::getInstance().getWindow().height();

  if (use)
    inspectionCameraHandler->resized(w, h);
  else
    userCameraHandler->resized(w, h);  
}

const PGUPV::Camera& BaseRenderer::getCamera() const {
  if (useInspectionCameraFlag)
    return inspectionCameraHandler->getCamera();
  return userCameraHandler->getCamera();
}

void BaseRenderer::update_camera(uint ms) {
  if (userCameraHandler)
    userCameraHandler->update(ms);
};

std::shared_ptr<Panel> BaseRenderer::addPanel(const std::string &title) {
  auto p = std::shared_ptr<Panel>(new Panel(title));
  panels.push_back(p);
  LIBINFO("Panel " + p->getName() + " creado");
  return p;
}

void BaseRenderer::postRender() {
  if (useInspectionCameraFlag) {
    userCameraHandler->getCamera().render();
  }
}

