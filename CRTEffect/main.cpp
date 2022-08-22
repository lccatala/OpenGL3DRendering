
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

Muestra una esfera texturada con una foto de la Tierra.

*/

class MyRender : public Renderer {
public:
  MyRender(){};
  void setup(void);
  void render(void);
  void reshape(uint w, uint h);
  void update(uint ms);

private:
  std::shared_ptr<GLMatrices> m_Mats;
  std::shared_ptr<Program> m_Program;
  std::shared_ptr<Texture2D> m_RockTexture;
  Sphere m_LavaSphere;
  float m_RockAngle;
};

void MyRender::setup() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  m_RockAngle = 0.0f;

  /* Este shader necesita las coordenadas de los vértices y sus 
  coordenadas de textura */
  m_Program = std::make_shared<Program>();
  m_Program->addAttributeLocation(Mesh::VERTICES, "position");
  m_Program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");

  m_Mats = GLMatrices::build();
  m_Program->connectUniformBlock(m_Mats, UBO_GL_MATRICES_BINDING_INDEX);

  m_Program->loadFiles("../CRTEffect/textureReplace");
  m_Program->compile();

  // Localización de los uniform (unidad de textura)
  GLint texUnitLoc = m_Program->getUniformLocation("texUnit");
  // Comunicamos la unidad de textura al shader
  m_Program->use();
  glUniform1i(texUnitLoc, 0);

  // Cargamos la nueva textura desde un fichero
  m_RockTexture = std::make_shared<Texture2D>();
  m_RockTexture->loadImage("../recursos/imagenes/tierra.jpg");
  m_RockTexture->bind(GL_TEXTURE0);

  setCameraHandler(std::make_shared<OrbitCameraHandler>());

  auto panel = addPanel("Controles");
  panel->setPosition(0.0f, 0.0f);
  panel->setSize(300.0f, 100.0f);
  panel->addWidget(std::make_shared<RGBColorWidget>("Color a filtrar", glm::vec3(1.0f, 1.0f, 1.0f), m_Program, "colorModifier"));
  App::getInstance().getWindow().showGUI(true);
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_Mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

  m_LavaSphere.render();

  CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  m_Mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

void MyRender::update(uint ms) {
  m_RockAngle += glm::radians(45.0f) * ms / 1000.0f;
  if (m_RockAngle > TWOPIf)
    m_RockAngle -= TWOPIf;
  m_Mats->setMatrix(GLMatrices::MODEL_MATRIX,
                  glm::rotate(glm::mat4(1.0f), m_RockAngle, vec3(0.1, 1.0, -0.1)));
}

int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
                                PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
