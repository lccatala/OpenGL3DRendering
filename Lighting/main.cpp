

#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"
#include "renderHelpers.h"

using namespace PGUPV;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using std::vector;

/*

Este ejemplo implementa una parte de la iluminación de la tubería fija de OpenGL
(sombreado de Gouraud con fuentes puntuales, sin atenuación)

*/


class MyRender : public Renderer {
public:
	MyRender()
		: cone(0.5f, 0.00001f, 1.0f, 10, 40),
		sph(0.5, 30, 30), box(0.2f, 1.0f, 0.5f), suelo(4, 4, vec4(0.0f), 10, 10),
		showAxes(false) {};
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;
	void update(uint) override;

private:
	// Uniform Buffer Objects
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<UBOLightSources> luces;

	Program gouraud, phong, constant;
	Cylinder cone;
	Sphere sph;
	Box box;
	Rect suelo;
	Axes axes;
	bool showAxes;
	std::shared_ptr<CheckBoxWidget> switchingWidget;

	void buildGUI();
	void updateLightPosition(const glm::mat4 &viewMatrix);
	void configureLights();
};

void MyRender::setup() {
	glClearColor(.7f, .7f, .7f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	/* Preparamos los materiales que usaremos en nuestra escena */
	auto plastico_rojo = PGUPV::getMaterial(PredefinedMaterial::RED_PLASTIC);
	auto plastico_verde = PGUPV::getMaterial(PredefinedMaterial::GREEN_PLASTIC);
	auto goma_azul = PGUPV::getMaterial(PredefinedMaterial::CYAN_RUBBER);
	auto perla = PGUPV::getMaterial(PredefinedMaterial::PEARL);

	//  Los asociamos a los modelos
	cone.accept([plastico_rojo](Mesh &m) { m.setMaterial(plastico_rojo); });
	sph.accept([plastico_verde](Mesh &m) { m.setMaterial(plastico_verde); });
	box.accept([goma_azul](Mesh &m) { m.setMaterial(goma_azul); });
	suelo.accept([perla](Mesh &m) { m.setMaterial(perla); });

	// Definimos la posición de los atributos
	gouraud.addAttributeLocation(Mesh::VERTICES, "position");
	gouraud.addAttributeLocation(Mesh::NORMALS, "normal");
	phong.addAttributeLocation(Mesh::VERTICES, "position");
	phong.addAttributeLocation(Mesh::NORMALS, "normal");

	// Para conectar un bloque de uniform con un shader que lo ha incluido
	// con una declaración tipo: $Material o $GLMatrices, se llama a
	// connectUniformBlock con un objeto UniformBufferObject y
	// con un índice del punto de vinculación de matrices, antes de compilar
	// el shader
	mats = GLMatrices::build();
	gouraud.connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	phong.connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);

	luces = UBOLightSources::build();
	gouraud.connectUniformBlock(luces, UBO_LIGHTS_BINDING_INDEX);
	phong.connectUniformBlock(luces, UBO_LIGHTS_BINDING_INDEX);

	// Creamos un material temporal para sustituir la declaración del shader
	gouraud.connectUniformBlock(UBOMaterial::build(), UBO_MATERIALS_BINDING_INDEX);
	phong.connectUniformBlock(UBOMaterial::build(), UBO_MATERIALS_BINDING_INDEX);

	// Este shader se encarga de calcular la iluminación, usando
	// el algoritmo de Gouraud 
	gouraud.loadFiles("../Lighting/gouraud");
	gouraud.compile();
	phong.loadFiles("../Lighting/phong");
	phong.compile();

	// Definimos la posición y atributos de las fuentes
	configureLights();

	/* Este shader aplica el color definido en el uniform primitive_color */
	constant.addAttributeLocation(Mesh::VERTICES, "position");
	constant.addAttributeLocation(Mesh::COLORS, "vertcolor");

	constant.connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	constant.loadFiles("../recursos/shaders/constantshadinguniform");
	constant.compile();

	setCameraHandler(std::make_shared<OrbitCameraHandler>());

	buildGUI();
}

// Define los parámetros de las luces
void MyRender::configureLights() {
	// Definición de los parámetros de la fuente
	LightSourceParameters lp;

	////////////////
	// Fuente 0. Puntual
	////////////////
	// Posición de la luz en el sistema de coordenadas del mundo
	lp.positionWorld = vec4(1.0, 1.0, 1.0, 1.0);

	lp.ambient = vec4(.1, .1, .1, 1.0);
	lp.diffuse = vec4(0.4, 0.4, 0.4, 1.0);
	lp.specular = vec4(0.8, 0.8, 0.8, 1.0);

	lp.spotCutoff = 180.0;
	lp.attenuation = vec3(1.1, 0.0, 0.0);
	// Actualizamos las características de la fuente 0
	luces->setLightSource(0, lp);

	////////////////
	// Fuente 1. Direccional
	////////////////

	lp = luces->getLightSource(1);

	lp.positionWorld = vec4(-3.0, 0.0, 0.0, 1.0);
	lp.ambient = vec4(.1, .1, .1, 1.0);
	lp.diffuse = vec4(0.42, 0.4, 0.6, 1.0);

	lp.directional = 1;
	lp.attenuation = vec3(1.1, 0.0, 0.0);

	luces->setLightSource(1, lp);

	////////////////
	// Fuente 2. Focal
	////////////////

	lp = luces->getLightSource(2);

	/* Posición de la luz en el sistema de coordenadas del mundo */
	lp.positionWorld = vec4(0.5, 1.15, 1.5, 1.0);
	// El foco apunta al origen.
	lp.spotDirectionWorld = glm::normalize(vec3(-0.5, -0.6, -0.14));

	lp.ambient = vec4(0.1, 0.1, 0.1, 1.0);
	lp.diffuse = vec4(0.2, 0.9, 0.2, 1.0);
	lp.specular = vec4(0.7, 0.7, 0.7, 1.0);
	// .x: spotExponent, .y: spotCutoff (degrees), .z: spotCosCutoff
	lp.spotExponent = 12;
	lp.spotCutoff = 25.f;
	lp.spotCosCutoff = cosf(glm::radians(lp.spotCutoff));
	// .x: constant, .y: linear, .z: quadratic
	// Inicialmente sin atenuación. Ajusta la atenuación a tu escena
	lp.attenuation = vec3(1.0, 0.0, 0.0);
	luces->setLightSource(2, lp);

	////////////////
	// Fuente 3. Focal
	////////////////

	lp = luces->getLightSource(3);

	/* Posición de la luz en el sistema de coordenadas del mundo */
	lp.positionWorld = vec4(1.53, 0.51, -1.86, 1.0);
	// El foco apunta al origen.
	lp.spotDirectionWorld = glm::normalize(vec3(-0.1, -0.24, 0.24));

	lp.ambient = vec4(0.1, 0.1, 0.1, 1.0);
	lp.diffuse = vec4(0.9, 0.9, 0.2, 1.0);
	lp.specular = vec4(0.7, 0.7, 0.1, 1.0);

	// .x: spotExponent, .y: spotCutoff (degrees), .z: spotCosCutoff
	lp.spotExponent = 50;
	lp.spotCutoff = 10.f;
	lp.spotCosCutoff = cosf(glm::radians(lp.spotCutoff));
	// .x: constant, .y: linear, .z: quadratic
	// Inicialmente sin atenuación. Ajusta la atenuación a tu escena
	lp.attenuation = vec3(0.57, 0.0, 0.0);
	luces->setLightSource(3, lp);
}


void MyRender::update(uint) {
	if (App::isKeyUp(KeyCode::A))
		showAxes = !showAxes;

	/*
	  Puedes añadir aquí tus propias teclas para controlar tu práctica

	  */
}

// Calculamos la posición y dirección del foco en el sistema de coordenadas de
// la cámara
void MyRender::updateLightPosition(const glm::mat4 &viewMatrix) {
	for (uint i = 0; i < luces->size(); i++) {
		LightSourceParameters lp = luces->getLightSource(i);
		// Posición de la luz en coordenadas de la cámara
		if (lp.directional)
			lp.positionEye = viewMatrix * vec4(vec3(lp.positionWorld), 0.0);
		else
			lp.positionEye = viewMatrix * lp.positionWorld;
		// spotDirection contiene la dirección en la que apunta la cámara, en
		// coordenadas de la cámara
		lp.spotDirectionEye =
			glm::normalize(vec3(viewMatrix * vec4(lp.spotDirectionWorld, 0)));
		luces->setLightSource(i, lp);
	}
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 viewMatrix = getCamera().getViewMatrix();
	mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);
	updateLightPosition(viewMatrix);

	// Primero dibujamos los objetos con iluminación
	if (switchingWidget->getValue().getValue()) {
		phong.use();
	}
	else {
		gouraud.use();
	}

	// Cono rojo
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->translate(GLMatrices::MODEL_MATRIX, 1.0, 0.0, -0.5);
	cone.render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	// Esfera verde
	sph.render();

	// Caja azul
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->translate(GLMatrices::MODEL_MATRIX, -0.75f, 0.0f, 0.4f);
	mats->rotate(GLMatrices::MODEL_MATRIX, glm::radians(150.0f), 0.0, 1.0, 0.0);
	box.render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	// Suelo
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->translate(GLMatrices::MODEL_MATRIX, 0.0, -0.5001f, 0.0);
	mats->rotate(GLMatrices::MODEL_MATRIX, glm::radians(-90.0f), 1.0, 0.0, 0.0);
	suelo.render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	// Dibujamos los ejes
	if (showAxes) {
		ConstantIllumProgram::use();
		axes.render();
	}
	// Dibujamos las fuentes
	PGUPV::renderLightSources(*mats, *luces);
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

// Añadimos un widget para controlar las luces de la escena
void MyRender::buildGUI() {
	auto panel = addPanel("Fuentes");
	panel->setPosition(5, 10);
	panel->setSize(280, 505);

	// Este widget permite definir los parámetros de las fuentes de luz
	panel->addWidget(std::make_shared<LightSourceWidget>(luces, 10.f, getCameraHandler()->getCameraPtr()));

	switchingWidget = std::make_shared<CheckBoxWidget>("Usar Phong", false);
	panel->addWidget(switchingWidget);
	App::getInstance().getWindow().showGUI(true);
}

int main(int argc, char *argv[]) {
	App &myApp = App::getInstance();
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
		PGUPV::MULTISAMPLE);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
