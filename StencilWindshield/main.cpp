#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <cstdlib>

#include "PGUPV.h"

using namespace PGUPV;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using std::vector;

class GameObject
{
public:
	void FromOBJ(std::string filepath);
	void Render(std::shared_ptr<GLMatrices> mats);
	void LoadTexture(const std::string& filepath);
	glm::vec3 Position = {0.0f,-4.0f,0.0f};
	float RotationAmmount = 0.0f;
	glm::vec3 RotationAxis = {0.0f, 1.0f, 0.0f};
	uint8_t Scale = 1;
	std::shared_ptr<Model> m_Model;
	std::shared_ptr<Texture2D> m_Texture;

private:
	std::map<std::string, glm::vec4> readMTL(std::string mtlFilename);
};

const float STARTING_Z = -40.0f;
const float STARTING_Z_ROAD = -60.0f;
const float STARTING_X_RIGHT = 16.0f;
const float TREES_DISTANCE = 10.0f;
const float MIN_TREE_CAMERA_DISTANCE = 0.0f;
const float MIN_ROAD_CAMERA_DISTANCE = 0.0f;
const float ROAD_SIDE = 90.0f;

class MyRender : public Renderer {
public:
	MyRender() {};
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;
	void update(uint ms) override;

	void buildModels();
private:
	std::shared_ptr<GLMatrices> mats;
	Axes axes;
	float rotationAngle = 0.0f;
	Model rectangle;

	std::vector<GameObject> trees;
	std::vector<GameObject> road;
	GameObject parabrisas;
	GameObject dashboard;
};

void GameObject::LoadTexture(const std::string& filepath)
{
	m_Texture = std::make_shared<Texture2D>();
	m_Texture->loadImage(filepath);
}

void GameObject::FromOBJ(std::string filepath)
{
	m_Model = std::make_shared<Model>();


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;
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
				vertices.push_back(vertex);
				colors.push_back(currentColor);

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
	
	auto mesh = std::make_shared<Mesh>();
	mesh->addVertices(vertices);

	DrawCommand* drawCommand = new DrawArrays(GL_TRIANGLES, 0, vertices.size());
	mesh->addDrawCommand(drawCommand);
	mesh->addColors(colors);

	m_Model->addMesh(mesh);
}

std::map<std::string, glm::vec4> GameObject::readMTL(std::string mtlFilename)
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

void GameObject::Render(std::shared_ptr<GLMatrices> mats)
{
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->scale(GLMatrices::MODEL_MATRIX, Scale);
	mats->translate(GLMatrices::MODEL_MATRIX, Position);
	mats->rotate(GLMatrices::MODEL_MATRIX, RotationAmmount, RotationAxis);

	m_Model->render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);
}

void MyRender::buildModels()
{
	float carZ = -1.0f;

	// Parabrisas
	parabrisas.FromOBJ("parabrisas2.obj");
	parabrisas.Position = {0.5f, -0.1f, carZ};
	parabrisas.Scale = 3;
	parabrisas.m_Model->getMesh(0).setColor({0.0f, 0.0f, 1.0f, 0.5f});

	// Arboles
	std::vector<std::string> modelNames = { "lowpolypine2.obj" , "tree2.obj", "Pino.obj"};
	std::vector<float> zStartingValues = {STARTING_Z, STARTING_Z + TREES_DISTANCE, STARTING_Z + TREES_DISTANCE  * 2};
	for (std::size_t i = 0; i < modelNames.size(); ++i)
	{
		GameObject go;
		go.FromOBJ(modelNames.at(i));
		go.Position.z = zStartingValues.at(i);
		go.Position.x = STARTING_X_RIGHT;
		trees.push_back(go);

		int pos = modelNames.size() - 1 - i;
		go.FromOBJ(modelNames.at(pos));
		go.Position.z = zStartingValues.at(i);
		go.Position.x = -STARTING_X_RIGHT;
		trees.push_back(go);
	}

	// Carretera
	for (int i = 0; i < 3; ++i)
	{
		GameObject r;
		r.m_Model = std::make_shared<Rect>(ROAD_SIDE, ROAD_SIDE);
		r.Position = { 0.0f, -10.0f, -ROAD_SIDE * i };
		r.RotationAmmount = glm::radians(-90.0f);
		r.RotationAxis = { 1.0f, 0.0f, 0.0f };
		glm::vec2 tc[] = {
			glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
		r.m_Model->getMesh(0).addTexCoord(0, tc, 4);
		r.LoadTexture("asfalto.png");
		road.push_back(r);
	}

	// Dashboard
	dashboard.m_Model = std::make_shared<Rect>(5.0f, 2.5f);
	dashboard.Position = {0.5f, -0.79f, carZ};
	dashboard.Scale = 1;
	glm::vec2 tcdb[] = {
	  glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
	  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };
	dashboard.m_Model->getMesh(0).addTexCoord(0, tcdb, 4);
	dashboard.LoadTexture("dashboard.png");
}

void MyRender::setup() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	mats = PGUPV::GLMatrices::build();

	buildModels();

	mats->setMatrix(GLMatrices::VIEW_MATRIX, glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3(0.0f, 0.0f, -1.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f)));
}

/*
Tienes que completar esta función para dibujar la escena.

*/
void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ConstantIllumProgram::use();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	for (auto& tree : trees)
	{
		tree.Render(mats);
	}


	// Dibujar carretera
	TextureReplaceProgram::use();
	for (auto& r : road)
	{
		r.m_Texture->bind(GL_TEXTURE0);
		r.Render(mats);
	}

	ConstantIllumProgram::use();

	glEnable(GL_BLEND);
	parabrisas.Render(mats);
	glDisable(GL_BLEND);

	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
	// Dibujar salpicadero
	TextureReplaceProgram::use();
	dashboard.m_Texture->bind(GL_TEXTURE0);
	dashboard.Render(mats);
	ConstantIllumProgram::use();

	glDisable(GL_STENCIL_TEST);

	CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
	if (h == 0)
		h = 1;
	float ar = (float)w / h;

	
	float fov = 70.0f;
	float near = 0.001f;
	float far = 1000.0f;
	//float ar = 16.0f / 9.0f;
	mats->setMatrix(
		GLMatrices::PROJ_MATRIX,
		glm::perspective(fov, ar, near, far));
	// Borrar el stencil buffer
	glClear(GL_STENCIL_BUFFER_BIT);
	// Esta condición del stencil siempre pasa
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	// En cualquier caso, siempre que toquemos un pixel, reemplazaremos el valor
	// de su stencil por el valor indicado
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// No necesitamos...
	glDisable(GL_DEPTH_TEST);                // z-buffer
	glColorMask(false, false, false, false); // escribir en el buffer de color

	// Activar el test del stencil
	glEnable(GL_STENCIL_TEST);


	// Desabilitar el test de stencil, activar la escritura al buffer de color, y
	// activar el z-buffer
	glEnable(GL_DEPTH_TEST);

	parabrisas.Render(mats);
	glDisable(GL_STENCIL_TEST);

	glColorMask(true, true, true, true);
	glEnable(GL_DEPTH_TEST);
}

// Radianes por segundo a los que giran los ejes
#define SPINSPEED glm::radians(90.0f)

void MyRender::update(uint ms) {
	/*
	rotationAngle += SPINSPEED * ms / 1000.0f;
	if (rotationAngle > TWOPIf) rotationAngle -= TWOPIf;
	*/

	float speed = 0.02f;
	for (auto& tree : trees)
	{
		if (tree.Position.z >= MIN_TREE_CAMERA_DISTANCE)
		{
			tree.Position.z = STARTING_Z;
			tree.RotationAmmount = (rand() % 628) / 100;
		}
		tree.Position.z += speed * ms;
	}

	for (auto& r : road)
	{
		if (r.Position.z >= MIN_ROAD_CAMERA_DISTANCE)
		{
			r.Position.z = -ROAD_SIDE * 2 + 1;
		}
		r.Position.z += speed * ms;
	}


	// Hay que llamar a reshape para que funcione. No sirve con llamarlo en setup()
	uint w = App::getInstance().getWindow().width();
	uint h = App::getInstance().getWindow().height();
	reshape(w, h);
}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER | PGUPV::MULTISAMPLE | PGUPV::STENCIL_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
