
#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"

using namespace PGUPV;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

using std::vector;


/*

Simulando un volumen de recorte cilíndrico con un shader de geometría

*/

#define INITIAL_RADIUS 0.25f
#define INITIAL_HEIGHT 0.5f
#define INITIAL_AXIS glm::vec3(0.0f, 1.0f, 0.0f)

class MyRender : public Renderer {
public:
  MyRender()
    :
    cylinderPosWCS(0.8f, 0.8f, 0.0f), // Posición en el S.C. del mundo
    cylinderModel(1.0f, 1.0f, 1.0f),
    // Este objeto interpola linealmente entre las dos primeras variables,
    // la tercera es la duración de un ciclo y la cuarta el tipo de
    // movimiento
    // Se usa para calcular la posición del cilindro en cada momento.
    li(cylinderPosWCS, vec3(-0.6, -0.4, 0), 4.0, PING_PONG) {};
  void setup(void) override;
  void render(void) override;
  void reshape(uint w, uint h) override;
  void update(uint ms) override;

private:
  std::shared_ptr<GLMatrices> mats;
  Axes axes;
  std::shared_ptr<Program> program;
  GLint lightPosLoc;
  vec4 lightPosWCS;
  GLint cylinderPosLoc;
  vec3 cylinderPosWCS; // Posición de la base del cilindro en el S.C. del mundo

  Cylinder cylinderModel;
  std::shared_ptr<Scene> bunny;
  LinearInterpolator<vec3> li;

  std::shared_ptr<FloatSliderWidget> cylinderHeight, cylinderRadius;
  std::shared_ptr<DirectionWidget> cylinderAxis; // Eje del cilindro en el S.C. del mundo
  void buildGUI();
};

void MyRender::update(uint ms) {
  static uint total = 0;

  total += ms;
  cylinderPosWCS = li.interpolate(total / 1000.0);
}

void MyRender::setup() {
  glClearColor(1.f, 1.f, 1.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  // Inicialización de variables de estado
  // Posición de la luz en el S.C. del mundo
  lightPosWCS = vec4(2.f, 2.f, 2.f, 1.f);

  bunny = FileLoader::load("../recursos/modelos/bunny.obj");

  /* Este shader se encarga de calcular la iluminación (sólo componente
   * difusa)*/
  program = std::make_shared<Program>();
  program->addAttributeLocation(Mesh::VERTICES, "position");
  program->addAttributeLocation(Mesh::NORMALS, "normal");

  mats = GLMatrices::build();
  program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
  program->loadFiles("../GeometryFiltering/GeometryFiltering");
  program->compile();

  // Posiciones de las variables uniform
  lightPosLoc = program->getUniformLocation("lightpos");
  cylinderPosLoc = program->getUniformLocation("cylinderPos");

  // Color del objeto
  program->use();
  GLint diffuseLoc = program->getUniformLocation("diffuseColor");
  glUniform4f(diffuseLoc, 0.8f, 0.8f, 0.8f, 1.0f);

  setCameraHandler(std::make_shared<OrbitCameraHandler>());

  buildGUI();
  App::getInstance().getWindow().showGUI();
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 viewMatrix = getCamera().getViewMatrix();
  mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);

  program->use();
  // Posición de la fuente en el S.C. de la cámara
  vec4 lp = viewMatrix * lightPosWCS;
  glUniform3f(lightPosLoc, lp.x, lp.y, lp.z);

  // Posición de la base del cilindro en el espacio del mundo
  glUniform3fv(cylinderPosLoc, 1, &cylinderPosWCS.x);

  // Dibujando el objeto
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.3f, 0.3f, 0.0f);
  mats->scale(GLMatrices::MODEL_MATRIX, vec3(1.2f / bunny->maxDimension()));
  mats->translate(GLMatrices::MODEL_MATRIX, -bunny->center());
  bunny->render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  // Un shader sencillo para dibujar el color definido por cada vértice
  ConstantIllumProgram::use();

  // Dibujamos los ejes de coordenadas
  axes.render();

  // Dibujamos el cilindro en alámbrico
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, cylinderPosWCS);

  GLStateCapturer<PolygonModeState> restorePolygonModeState;
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  auto t = cylinderAxis->get();
  auto yp1 = t.y + 1.0f;
  mats->multMatrix(GLMatrices::MODEL_MATRIX, glm::mat4(
    t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1, 0.0f,
    t.x, t.y, t.z, 0.0f,
    -t.z*t.x / yp1, -t.z, t.y + (t.x*t.x / yp1), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  ));
  vec3 cylScale(cylinderRadius->get());
  cylScale.y = cylinderHeight->get();
  mats->scale(GLMatrices::MODEL_MATRIX, cylScale);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.0f, 0.5f, 0.0f);
  cylinderModel.render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  if (h == 0)
    h = 1;
  float ar = (float)w / h;
  mats->setMatrix(GLMatrices::PROJ_MATRIX,
    glm::perspective(glm::radians(60.0f), ar, .1f, 100.0f));
  mats->loadIdentity(GLMatrices::MODEL_MATRIX);
}

void MyRender::buildGUI() {
  // Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
  auto panel = addPanel("Cilindro");

  // Podemos darle un tamaño y una posición, aunque no es necesario
  panel->setPosition(5, 50);
  panel->setSize(150, 240);

  cylinderRadius = std::make_shared<FloatSliderWidget>("Radio", INITIAL_RADIUS, 0.001f, 2.0f,
    program, "cylinderRadius");
  panel->addWidget(cylinderRadius);

  cylinderHeight = std::make_shared<FloatSliderWidget>("Altura", INITIAL_HEIGHT, 0.001f, 2.0f,
    program, "cylinderHeight");
  panel->addWidget(cylinderHeight);

  cylinderAxis = std::make_shared<DirectionWidget>("Eje", INITIAL_AXIS, program, "cylinderAxis");
  cylinderAxis->setCamera(getCameraHandler()->getCameraPtr());
  panel->addWidget(cylinderAxis);

}

int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
    PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
