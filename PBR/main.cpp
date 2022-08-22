#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"

using namespace PGUPV;

class MyRender : public Renderer {
public:
	MyRender() : axes(1.2f), sphere(1.0f, 20U, 40U), luminaire(.08f, 10, 10, glm::vec4(1.0, 1.0, 0.0, 1.0)), box(4.0f, .01f, 4.0f) {};
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;

private:
	void buildGUI();
	Axes axes;
	std::shared_ptr<Program> pbr_program;
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<UBOPBRLightSources> lights;

	Sphere sphere, luminaire;
	Box box;
	std::shared_ptr<Scene> model;
};

std::shared_ptr<Texture2D> loadTexture(const std::string& filename) {
	auto t = std::make_shared<Texture2D>(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	t->loadImage(filename);
	t->generateMipmap();
	return t;
}

std::shared_ptr<Scene> loadPBRPistol() {

	auto model = FileLoader::load("../recursos/modelos/Cerberus_LP.FBX");
	auto mat = std::make_shared<PBRMaterial>();

	mat->setBaseColorTexture(loadTexture("../recursos/modelos/Textures/Cerberus_A.png"));
	mat->setMetalnessTexture(loadTexture("../recursos/modelos/Textures/Cerberus_M.png"));
	mat->setNormalMapTexture(loadTexture("../recursos/modelos/Textures/Cerberus_N.png"));
	mat->setRoughnessTexture(loadTexture("../recursos/modelos/Textures/Cerberus_R.png"));
	mat->setAmbientOcTexture(loadTexture("../recursos/modelos/Textures/Cerberus_AO.png"));
	assert(model->getNumMaterials() == 1);

	model->processMeshes([mat](Mesh& m) {
		m.setMaterial(mat);
		});

	model->setMaterial(0, mat);
	return model;
}

void MyRender::setup() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Construímos el objeto GLMatrices
	pbr_program = std::make_shared<Program>();
	pbr_program->addAttributeLocation(Mesh::VERTICES, "position");
	pbr_program->addAttributeLocation(Mesh::NORMALS, "normal");
	pbr_program->addAttributeLocation(Mesh::TEX_COORD0, "texcoords");
	pbr_program->addAttributeLocation(Mesh::TANGENTS, "tangent");

	lights = UBOPBRLightSources::build();
	pbr_program->connectUniformBlock(lights, UBO_PBR_LIGHTS_BINDING_INDEX);
	mats = GLMatrices::build();
	pbr_program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	pbr_program->replaceString("$" + UBOPBRMaterial::blockName, UBOPBRMaterial::definition);
	pbr_program->loadFiles("../PBR/pbr-mat");
	pbr_program->compile();

	for (int i = 0; i < 4; i++) {
		auto l = lights->getLightSource(i);
		l.color = glm::vec3(1.0f, 1.0f, 1.0f);
		l.intensity = 20.0f + i * 5;
		l.scaledColor = l.color * l.intensity;
		if (i == 0) l.positionWorld = glm::vec4(-10.0f, 10.0f, 10.0f, 1.0f);
		else if (i == 1) l.positionWorld = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
		else if (i == 2) l.positionWorld = glm::vec4(-10.0f, -10.0f, 10.0f, 1.0f);
		else l.positionWorld = glm::vec4(10.0f, -10.0f, 10.0f, 1.0f);
		l.enabled = true;
		lights->setLightSource(i, l);
	}


	model = loadPBRPistol();

	setCameraHandler(std::make_shared<OrbitCameraHandler>());
	buildGUI();
	App::getInstance().getWindow().showGUI(true);
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewMatrix = getCamera().getViewMatrix();
	mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);

	lights->updateLightEyeSpacePosition(viewMatrix);

	pbr_program->use();

	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->scale(GLMatrices::MODEL_MATRIX, .1f, .1f, .1f);
	model->render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);


	ConstantUniformColorProgram::use();
	for (int i = 0; i < 4; i++) {
		mats->loadIdentity(GLMatrices::MODEL_MATRIX);
		mats->translate(GLMatrices::MODEL_MATRIX, lights->getLightSource(i).positionWorld);
		ConstantUniformColorProgram::setColor(glm::vec4(lights->getLightSource(i).color, 1.0));
		luminaire.render();
		mats->loadIdentity(GLMatrices::MODEL_MATRIX);
	}
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
	if (h == 0)
		h = 1;
	float ar = (float)w / h;
	// Definimos una cámara perspectiva con la misma razón de aspecto que la
	// ventana
	mats->setMatrix(GLMatrices::PROJ_MATRIX,
		glm::perspective(glm::radians(60.0f), ar, .1f, 30.0f));
}

void MyRender::buildGUI() {
	auto panel = addPanel("Luces");
	panel->setSize(260, 300);
	panel->setPosition(530, 10);

	auto lightProp = std::make_shared<PBRLightSourceWidget>(lights, 30.0f);
	panel->addWidget(lightProp);

}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
