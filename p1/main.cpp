#include <string>
#include <sstream>
#include <fstream>
#include <map>

#include <PGUPV.h>
#include <GUI3.h>

using namespace PGUPV;

using glm::vec3;
using std::vector;

struct ModelData
{
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec4> Colors;
};

class MyRender : public Renderer {
public:
	MyRender() = default;
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;
	void update(uint) override;
private:
	ModelData MyRender::readOBJ(std::string filepath);
	std::map<std::string, glm::vec4> readMTL(std::string mtlFilename);

	void buildModels();
	std::shared_ptr<GLMatrices> mats;
	Axes axes;
	std::vector<std::shared_ptr<Model>> leftModels;

	void buildGUI();
	std::shared_ptr<IntSliderWidget> modelSelector;
};

std::map<std::string, glm::vec4> MyRender::readMTL(std::string mtlFilename)
{
	std::string materialName;
	std::map<std::string, glm::vec4> materials;
	std::ifstream mtlInfile(mtlFilename.c_str());
	std::string mtlLine;
	while (std::getline(mtlInfile, mtlLine))
	{
		if (mtlLine.length() < 2)
			continue;

		if (mtlLine[0] == 'n')
		{
			std::size_t firstSpace = mtlLine.find_first_of(' ');
			materialName = mtlLine.substr(firstSpace + 1, mtlLine.length());
		}
		else if (mtlLine[0] == 'K' && mtlLine[1] == 'd')
		{
			std::size_t delim1 = 3;
			std::size_t delim2 = mtlLine.find(' ', delim1 + 1);
			std::size_t delim3 = mtlLine.find(' ', delim2 + 1);
			double r = std::stod(mtlLine.substr(delim1, delim2));
			double g = std::stod(mtlLine.substr(delim2, delim3));
			double b = std::stod(mtlLine.substr(delim3, mtlLine.length()));
			materials.insert({ materialName, {r, g, b, 1.0} });
		}
	}
	return materials;
}

ModelData MyRender::readOBJ(std::string filepath)
{
	ModelData modelData;
	int indexCounter = 0;
	int colorsIndex = 0;
	std::vector<glm::vec3> allVertices;
	glm::vec4 currentColor;
	std::map<std::string, glm::vec4> materials;
	std::ifstream infile(filepath.c_str());
	std::string line;
	while (std::getline(infile, line))
	{
		if (line.length() < 2)
			continue;

		if (line[0] == 'v' && line[1] == ' ') // Save all vertices
		{
			std::size_t delim1 = line.find(' ', 2);
			std::size_t delim2 = line.find(' ', delim1 + 1);

			double x = std::stod(line.substr(1, delim1));
			double y = std::stod(line.substr(delim1, delim2));
			double z = std::stod(line.substr(delim2, line.length()));

			allVertices.push_back({ x,y,z });
		}
		else if (line[0] == 'f') // Save vertices in indexed order
		{
			std::size_t prevSpacePos = 0;
			while (true)
			{
				std::size_t spacePos = line.find(' ', prevSpacePos);
				if (spacePos == std::string::npos)
					break;
				std::size_t slashPos = line.find('/', spacePos);

				int index = std::stoi(line.substr(spacePos, slashPos));
				auto vertex = allVertices.at(index - 1); // OBJs start indexing at 1 instead of 0
				modelData.Vertices.push_back(vertex);
				modelData.Colors.push_back(currentColor);

				prevSpacePos = spacePos + 1;
			}
		}
		else if (line[0] == 'm') // Read mtl file
		{
			std::size_t firstSpace = line.find_first_of(' ');
			std::string mtlFilename = line.substr(firstSpace + 1, line.length());

			std::size_t lastSlash = filepath.find_last_of('/');
			if (lastSlash != std::string::npos)
				mtlFilename = filepath.substr(0, lastSlash + 1) + mtlFilename;

			materials = readMTL(mtlFilename);
		}
		else if (line[0] == 'u')
		{
			std::size_t firstSpace = line.find_first_of(' ');
			std::string materialName = line.substr(firstSpace + 1, line.length());
			currentColor = materials.at(materialName);
		}
	}
	return modelData;
}

/**
Construye tus modelos y añádelos al vector models (quita los ejemplos siguientes). Recuerda
que no puedes usar directamente las clases predefinidas (tienes que construir los meshes y los
models a base de vértices, colores, etc.)
*/
void MyRender::buildModels()
{
	/*
	auto caja1 = std::make_shared<Box>(0.8f, 0.8f, 0.8f, glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
	models.push_back(caja1);
	auto caja2 = std::make_shared<Box>(0.9f, 0.9f, 0.9f, glm::vec4(0.2f, 0.8f, 0.2f, 1.0f));
	models.push_back(caja2);
	auto caja3 = std::make_shared<Box>(0.7f, 0.7f, 0.7f, glm::vec4(0.2f, 0.2f, 0.8f, 1.0f));
	models.push_back(caja3);
	*/

	ModelData modelData = readOBJ("lowpolypine2.obj");

	auto treeMesh = std::make_shared<Mesh>();
	treeMesh->addVertices(modelData.Vertices);

	DrawCommand* drawCommand = new DrawArrays(GL_TRIANGLES, 0, modelData.Vertices.size());
	treeMesh->addDrawCommand(drawCommand);
	treeMesh->addColors(modelData.Colors);

	std::shared_ptr<Model> treeModel = std::make_shared<Model>();
	treeModel->addMesh(treeMesh);
	leftModels.push_back(treeModel);
}


void MyRender::setup() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Habilitamos el z-buffer
	glEnable(GL_DEPTH_TEST);
	// Habilitamos el back face culling. ¡Cuidado! Si al dibujar un objeto hay 
	// caras que desaparecen o el modelo se ve raro, puede ser que estés 
	// definiendo los vértices del polígono del revés (en sentido horario)
	// Si ese es el caso, invierte el orden de los vértices.
	// Puedes activar/desactivar el back face culling en cualquier aplicación 
	// PGUPV pulsando las teclas CTRL+B
	glEnable(GL_CULL_FACE);

	mats = PGUPV::GLMatrices::build();

	// Activamos un shader que dibuja cada vértice con su atributo color
	ConstantIllumProgram::use();

	buildModels();

	// Establecemos una cámara que nos permite explorar el objeto desde cualquier
	// punto
	setCameraHandler(std::make_shared<OrbitCameraHandler>());

	// Construimos la interfaz de usuario
	buildGUI();
}

void MyRender::render() {
	// Borramos el buffer de color y el zbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Le pedimos a la cámara que nos de la matriz de la vista, que codifica la
	// posición y orientación actuales de la cámara.
	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());


	// Dibujamos los ejes
	axes.render();

	// Dibujamos los objetos
	if (!leftModels.empty()) {
		leftModels[modelSelector->get()]->render();
	}

	// Si la siguiente llamada falla, quiere decir que OpenGL se encuentra en
	// estado erróneo porque alguna de las operaciones que ha ejecutado
	// recientemente (después de la última llamada a CHECK_GL) ha tenido algún
	// problema. Revisa tu código.
	CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

// Este método se ejecuta una vez por frame, antes de llamada a render. Recibe el 
// número de milisegundos que han pasado desde la última vez que se llamó, y se suele
// usar para hacer animaciones o comprobar el estado de los dispositivos de entrada
void MyRender::update(uint) {
	// Si el usuario ha pulsado el espacio, ponemos la cámara en su posición inicial
	if (App::isKeyUp(PGUPV::KeyCode::Space)) {
		getCameraHandler()->resetView();
	}
}


/**
En éste método construimos los widgets que definen la interfaz de usuario. En esta
práctica no tienes que modificar esta función.
*/
void MyRender::buildGUI() {
	auto panel = addPanel("Modelos");
	modelSelector = std::make_shared<IntSliderWidget>("Model", 0, 0, static_cast<int>(leftModels.size() - 1));

	if (leftModels.size() <= 1) {
		panel->addWidget(std::make_shared<Label>("Introduce algún modelo más en el vector models..."));
	}
	else {
		panel->addWidget(modelSelector);
	}
	App::getInstance().getWindow().showGUI();
}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
		PGUPV::MULTISAMPLE);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
