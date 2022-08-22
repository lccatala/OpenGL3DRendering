#include <PGUPV.h>

using namespace PGUPV;

/*
Rellena las funciones setup y render tal y como se explica en el enunciado de la práctica.
¡Cuidado! NO uses las llamadas de OpenGL directamente (glGenVertexArrays, glBindBuffer, etc.).
Usa las clases Model y Mesh de PGUPV.
*/

class MyRender : public Renderer {
public:
	void setup(void);
	void render(void);
	void reshape(uint w, uint h);

private:
	Program mProgram;
	Model mModel;
	std::shared_ptr<Mesh> mMesh;
	const uint16_t TOTAL_VERTICES = 6;
	const float SIDE_SIZE = 0.9f;
	const float POSITION_OFFSET = 0.05f;
};

void MyRender::setup() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	mProgram.addAttributeLocation(0, "vPosition");
	mProgram.loadFiles("triangles");
	mProgram.compile();
	mProgram.use();

	glm::vec3 vertices[] = {
		{SIDE_SIZE - POSITION_OFFSET, -SIDE_SIZE, 0.0},
		{-SIDE_SIZE, SIDE_SIZE - POSITION_OFFSET, 0.0},
		{-SIDE_SIZE, -SIDE_SIZE, 0.0},

		{SIDE_SIZE, -SIDE_SIZE + POSITION_OFFSET, 0.0},
		{SIDE_SIZE, SIDE_SIZE, 0.0},
		{-SIDE_SIZE + POSITION_OFFSET, SIDE_SIZE, 0.0}
	};

	mMesh = std::make_shared<Mesh>();
	mMesh->addVertices(vertices, TOTAL_VERTICES);

	mModel.addMesh(mMesh);
	DrawCommand* drawCommand = new DrawArrays(GL_TRIANGLES, 0, TOTAL_VERTICES);
	mMesh->addDrawCommand(drawCommand);
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	mModel.render();
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}