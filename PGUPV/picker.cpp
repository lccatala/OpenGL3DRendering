#include "picker.h"
#include <fbo.h>
#include <texture2D.h>
#include <glStateCache.h>
#include <glMatrices.h>
#include <program.h>
#include <model.h>
#include <camera.h>

#include <glm/gtc/matrix_transform.hpp>

using PGUPV::Picker;
using PGUPV::Texture2D;
using PGUPV::GLMatrices;
using PGUPV::Model;


class Picker::PickerImpl {
public:
	PickerImpl();
	~PickerImpl() {};
	uint32_t pick(const PickData& pick, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const std::vector<ModelId>& objects);
	static glm::mat4 adjustProjMatrix(const PickData& pick, const glm::mat4& projMatrix);
protected:
	void prepareFBO();
	FBO fbo;
	static const int selectionWindowSemiWidth = 2;
	Program prog;
	GLint idLoc, mvpLoc;
};


Picker::Picker() : pimpl(new Picker::PickerImpl())
{
}

Picker::~Picker()
{
}

uint32_t Picker::pick(const PickData& pick, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const std::vector<ModelId>& objects)
{
	return pimpl->pick(pick, viewMatrix, projMatrix, objects);
}

glm::mat4 PGUPV::Picker::adjustProjMatrix(const PickData& pick, const glm::mat4& projMatrix)
{
	return Picker::PickerImpl::adjustProjMatrix(pick, projMatrix);
}

static const std::vector<std::string> pickerVert{
"#version 420 core",
"uniform mat4 mvp;",
"in vec4 position;",
"void main() {",
"  gl_Position = mvp * position;",
"}"
};

static const std::vector<std::string> pickerFrag{
"#version 420 core",
"uniform uint id;",
"out uint fragColor;",
"void main() {",
"  fragColor = id;",
"}"
};


Picker::PickerImpl::PickerImpl()
{
	prog.addAttributeLocation(PGUPV::Mesh::VERTICES, "position");
	prog.loadStrings(pickerVert, pickerFrag);
	prog.compile();
	idLoc = prog.getUniformLocation("id");
	mvpLoc = prog.getUniformLocation("mvp");
	prepareFBO();
}

uint32_t Picker::PickerImpl::pick(const Picker::PickData &pick, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const std::vector<ModelId>& objects)
{
	PGUPV::GLStateCapturer<PGUPV::FrameBufferObjectState<GL_DRAW_BUFFER>> currentFBO;
	PGUPV::CurrentProgramState currentProgram;

	fbo.bind();

	GLuint backgroundId[]{ 0, 0, 0, 0 };
	glClearBufferuiv(GL_COLOR, 0, backgroundId);
	float one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	auto adjustedProjMatrix = adjustProjMatrix(pick, projMatrix);
	auto vp = adjustedProjMatrix * viewMatrix;

	prog.use();

	glm::mat4 mvp;
	for (const auto &m : objects) {
		mvp = vp * m.modelMatrix;
		glUniform1ui(idLoc, m.id);
		glUniformMatrix4fv(mvpLoc, 1, false, &mvp[0][0]);
		m.m->render();
	}

	int selectionWindowSide = selectionWindowSemiWidth * 2 + 1;
	std::vector<GLuint> ids;
	ids.resize(selectionWindowSide * selectionWindowSide);
	glGetTextureImage(fbo.getAttachedTexture(GL_COLOR_ATTACHMENT0)->getId(),
		0,
		GL_RED_INTEGER,
		GL_UNSIGNED_INT,
		selectionWindowSide * selectionWindowSide * sizeof(GLuint),
		&ids[0]);

	for (int i = 0; i < selectionWindowSide * selectionWindowSide; i++) {
		INFO(std::to_string(ids[i]));
	}

	return ids[selectionWindowSemiWidth * selectionWindowSide + selectionWindowSemiWidth];
}

void Picker::PickerImpl::prepareFBO()
{
	auto tex = std::make_shared<Texture2D>(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	auto selectionWindowSide = 2 * selectionWindowSemiWidth + 1;
	tex->allocate(selectionWindowSide, selectionWindowSide, GL_R32UI);
	fbo.attach(GL_COLOR_ATTACHMENT0, tex);
	fbo.createAndAttach(GL_DEPTH_ATTACHMENT, selectionWindowSide, selectionWindowSide);
	assert(fbo.isComplete());
}

glm::mat4 Picker::PickerImpl::adjustProjMatrix(const PickData& pick, const glm::mat4& projMatrix)
{
	auto view = PGUPV::Camera::decomposeProjMatrix(projMatrix);

	if (view.type == Camera::CameraType::Unknown) {
		ERRT("The projection matrix cannot be decomposed for picking");
	}
	float newLeft = view.left + (view.right - view.left) * (pick.x - selectionWindowSemiWidth) / pick.width;
	float newRight = view.right - (view.right - view.left) * (pick.width - pick.x - 1 - selectionWindowSemiWidth) / pick.width;

	auto y_bottomleft = pick.height - pick.y;
	float newBottom = view.bottom + (view.top - view.bottom) * (y_bottomleft - selectionWindowSemiWidth) / pick.height;
	float newTop = view.top - (view.top - view.bottom) * (pick.height - y_bottomleft - 1 - selectionWindowSemiWidth) / pick.height;

	if (view.type == Camera::CameraType::Perspective)
		return glm::frustum(newLeft, newRight, newBottom, newTop, view.near, view.far);
	else 
		return glm::ortho(newLeft, newRight, newBottom, newTop, view.near, view.far);
}
