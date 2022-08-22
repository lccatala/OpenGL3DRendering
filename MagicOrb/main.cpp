
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
    MyRender() {};
    void setup(void);
    void render(void);
    void reshape(uint w, uint h);
    void update(uint ms);

private:
    std::shared_ptr<GLMatrices> m_Mats;
    std::shared_ptr<Program> m_Program;
    std::shared_ptr<Texture2D> m_RockTexture, m_GrassTexture, m_CloudsTexture, m_CoreTexture;
    Sphere m_LavaSphere, m_WhiteSphere, m_CloudsSphere, m_CoreSphere;
    float m_RockAngle;

    float m_LavaSphereScale, m_LavaSphereDeltaScale, m_LavaMinScale, m_LavaMaxScale;
    float m_CloudsSphereScale, m_CoreSphereScale;
};

void MyRender::setup() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_RockAngle = 0.0f;
    m_LavaSphereScale = 1.0f;
    m_LavaSphereDeltaScale = -0.1f;
    m_LavaMinScale = 1.0f;
    m_LavaMaxScale = 1.3f;
    m_CloudsSphereScale = 0.9f;
    m_CoreSphereScale = 0.5f;

    /* Este shader necesita las coordenadas de los vértices y sus
    coordenadas de textura */
    m_Program = std::make_shared<Program>();
    m_Program->addAttributeLocation(Mesh::VERTICES, "position");
    m_Program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");

    m_Mats = GLMatrices::build();
    m_Program->connectUniformBlock(m_Mats, UBO_GL_MATRICES_BINDING_INDEX);

    m_Program->loadFiles("../MagicOrb/textureReplace");
    m_Program->compile();

    // Localización de los uniform (unidad de textura)
    GLint texUnit0Loc = m_Program->getUniformLocation("texUnit0");
    // Comunicamos la unidad de textura al shader
    m_Program->use();
    glUniform1i(texUnit0Loc, 0);

    // Cargamos la nueva textura desde un fichero
    m_RockTexture = std::make_shared<Texture2D>();
    m_RockTexture->loadImage("lava.jpg");

    m_GrassTexture = std::make_shared<Texture2D>();
    m_GrassTexture->loadImage("grass.jpg");

    m_CloudsTexture = std::make_shared<Texture2D>();
    m_CloudsTexture->loadImage("clouds.jpg");

    m_CoreTexture = std::make_shared<Texture2D>();
    m_CoreTexture->loadImage("white.jpg");

    setCameraHandler(std::make_shared<OrbitCameraHandler>());
    auto panel = addPanel("Controles");
    panel->setPosition(0.0f, 0.0f);
    panel->setSize(300.0f, 100.0f);
    panel->addWidget(std::make_shared<RGBColorWidget>("Color a filtrar", glm::vec3(0.6, 0.5, 0.2), m_Program, "filterColor"));
    App::getInstance().getWindow().showGUI(true);
}

void MyRender::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

    m_GrassTexture->bind(GL_TEXTURE0);
    m_Mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    m_Mats->scale(GLMatrices::MODEL_MATRIX, m_LavaSphereScale);
    m_Mats->translate(GLMatrices::MODEL_MATRIX, {0.0f, 0.0f, 0.0f});
    m_Mats->rotate(GLMatrices::MODEL_MATRIX, m_RockAngle, { 0.0f, 2.0f, 1.0f });
    m_WhiteSphere.render();
    m_Mats->popMatrix(GLMatrices::MODEL_MATRIX);

    m_RockTexture->bind(GL_TEXTURE0);
    m_Mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    m_Mats->scale(GLMatrices::MODEL_MATRIX, m_LavaMinScale);
    m_Mats->translate(GLMatrices::MODEL_MATRIX, { 0.0f, 0.0f, 0.0f });
    m_Mats->rotate(GLMatrices::MODEL_MATRIX, m_RockAngle, { -0.3f, -1.0f, 0.0f });
    m_LavaSphere.render();
    m_Mats->popMatrix(GLMatrices::MODEL_MATRIX);

    m_CloudsTexture->bind(GL_TEXTURE0);
    m_Mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    m_Mats->scale(GLMatrices::MODEL_MATRIX, m_CloudsSphereScale);
    m_Mats->translate(GLMatrices::MODEL_MATRIX, { 0.0f, 0.0f, 0.0f });
    m_Mats->rotate(GLMatrices::MODEL_MATRIX, m_RockAngle, { 0.5f, 1.0f, 0.0f });
    m_CloudsSphere.render();
    m_Mats->popMatrix(GLMatrices::MODEL_MATRIX);

    m_CoreTexture->bind(GL_TEXTURE0);
    m_Mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    m_Mats->scale(GLMatrices::MODEL_MATRIX, m_CoreSphereScale);
    m_Mats->translate(GLMatrices::MODEL_MATRIX, { 0.0f, 0.0f, 0.0f });
    m_CoreSphere.render();
    m_Mats->popMatrix(GLMatrices::MODEL_MATRIX);
    
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
    m_LavaSphereScale += m_LavaSphereDeltaScale * ms / 1000.0f;
    if (m_LavaSphereScale > m_LavaMaxScale) {
        m_LavaSphereDeltaScale *= -1.0f;
        m_LavaSphereScale = 1.3f;
    } else if (m_LavaSphereScale < m_LavaMinScale) {
        m_LavaSphereDeltaScale *= -1.0f;
        m_LavaSphereScale = m_LavaMinScale;
    }
}

int main(int argc, char* argv[]) {
    App& myApp = App::getInstance();
    myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
        PGUPV::MULTISAMPLE);
    myApp.getWindow().setRenderer(std::make_shared<MyRender>());
    return myApp.run();
}
