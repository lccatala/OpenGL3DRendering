#include "camera.h"
#include "model.h"
#include "stockModels.h"
#include "indexedBindingPoint.h"
#include "stockPrograms.h"
#include "drawCommand.h"                      // for DrawElements

using glm::mat4;
using glm::vec3;

using PGUPV::Camera;
using PGUPV::GLMatrices;
using PGUPV::Mesh;

std::unique_ptr<PGUPV::Axes> Camera::axes;

Camera::~Camera() = default;

glm::vec3 Camera::getUpDirection() const {
	return glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
}

glm::vec3 Camera::getViewDirection() const {
	return glm::vec3(-viewMatrix[0][2], -viewMatrix[1][2], -viewMatrix[2][2]);
}

glm::vec3 Camera::getRightDirection() const {
	return glm::cross(getViewDirection(), getUpDirection());
}

// From: https://www.opengl.org/discussion_boards/showthread.php/178484-Extracting-camera-position-from-a-ModelView-Matrix
glm::vec3 Camera::getCameraPos() const {
	// Get the 3 basis vector planes at the camera origin and transform them into model space.
	//  
	// NOTE: Planes have to be transformed by the inverse transpose of a matrix
	//       Nice reference here: http://www.opengl.org/discussion_boards/showthread.php/159564-Clever-way-to-transform-plane-by-matrix
	//
	//       So for a transform to model space we need to do:
	//            inverse(transpose(inverse(MV)))
	//       This equals : transpose(MV) - see Lemma 5 in http://mathrefresher.blogspot.com.au/2007/06/transpose-of-matrix.html
	//
	// As each plane is simply (1,0,0,0), (0,1,0,0), (0,0,1,0) we can pull the data directly from the transpose matrix.
	//  
	mat4 modelViewT = glm::transpose(viewMatrix);

	// Get plane normals 
	vec3 n1(modelViewT[0]);
	vec3 n2(modelViewT[1]);
	vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	// Get the intersection of these 3 planes 
	// (uisng math from RealTime Collision Detection by Christer Ericson)
	vec3 n2n3 = glm::cross(n2, n3);
	float denom = glm::dot(n1, n2n3);

	vec3 top = (n2n3 * d1) + glm::cross(n1, (d3 * n2) - (d2 * n3));
	return top / -denom;
}

float Camera::getFOV() const {
	float fov = 2.f * atanf(1.0f / projMatrix[1][1]);
	return fov;
}

float Camera::getAspectRatio() const {
	return projMatrix[1][1] / projMatrix[0][0];
}

float Camera::getNear() const {
	float near = (2.0f * projMatrix[3][2]) / (2.0f * projMatrix[2][2] - 2.0f);
	return near;
}

float Camera::getFar() const {
	float far = ((projMatrix[2][2] - 1.0f) * getNear()) / (projMatrix[2][2] + 1.0f);
	return far;
}

void Camera::setViewMatrix(const glm::mat4 & m) {
	viewMatrix = m;
}

Camera::ViewVolume Camera::decomposeProjMatrix(const glm::mat4 & proj)
{
	Camera::ViewVolume result;
	if (proj[2][3] == 0.0f && proj[3][3] == 1.0f) {
		result.type = CameraType::Orthographic;
		result.near = (1 + proj[3][2]) / proj[2][2];
		result.far = -(1 - proj[3][2]) / proj[2][2];
		result.top = (1 - proj[3][1]) / proj[1][1];
		result.bottom = -(1 + proj[3][1]) / proj[1][1];
		result.left = -(1 + proj[3][0]) / proj[0][0];
		result.right = (1 - proj[3][0]) / proj[0][0];

	}
	else if (proj[2][3] == -1.0f && proj[3][3] == 0.0f) {
		result.type = CameraType::Perspective;
		result.near = proj[3][2] / (proj[2][2] - 1);
		result.far = proj[3][2] / (proj[2][2] + 1);
		result.bottom = result.near * (proj[2][1] - 1) / proj[1][1];
		result.top = result.near * (proj[2][1] + 1) / proj[1][1];
		result.left = result.near * (proj[2][0] - 1) / proj[0][0];
		result.right = result.near * (proj[2][0] + 1) / proj[0][0];
	}
	else {
		result.type = CameraType::Unknown;
	}
	return result;
}

void Camera::updateVerticesInFrustum() const {

	glm::mat4 ip = glm::inverse(projMatrix);

	glm::vec4 trnear = ip * glm::vec4(1.0f, 1.0f, -1.0, 1.0);
	glm::vec4 blnear = ip * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0);

	glm::vec4 trfar = ip * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 blfar = ip * glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);

	trnear = trnear / trnear.w;
	blnear = blnear / blnear.w;

	trfar = trfar / trfar.w;
	blfar = blfar / blfar.w;

	axesScale = (trfar.z - trnear.z) / 10.0f;
	std::vector<glm::vec4> vertices{
	blnear,
	glm::vec4(trnear.x, blnear.y, blnear.z, 1.0),
	trnear,
	glm::vec4(blnear.x, trnear.y, trnear.z, 1.0),

	blfar,
	glm::vec4(trfar.x, blfar.y, blfar.z, 1.0),
	trfar,
	glm::vec4(blfar.x, trfar.y, trfar.z, 1.0)
	};

	auto& mesh = frustum->getMesh(0);
	auto bo = mesh.getBufferObject(Mesh::BufferObjectType::VERTICES);
	gl_copy_write_buffer.bind(bo);
	gl_copy_write_buffer.write(&vertices[0].x, static_cast<unsigned int>(sizeof(glm::vec4) * vertices.size()), 0);
}


void Camera::setProjMatrix(const glm::mat4 & m) {
	projMatrix = m;
	recomputeFrustum = true;
}

void Camera::render() const {
	if (!frustum) {
		auto mesh = std::make_shared<Mesh>();
		mesh->addVertices(std::vector<glm::vec4>(8, glm::vec4(0.0f)));

		std::vector<ushort> indices{
		  0, 4, 5, 1, 0, 3, 7, 4, 0xFFFF,
		  6, 2, 3, 7, 6, 5, 1, 2
		};

		mesh->addIndices(indices);
		mesh->setColor(frustumColor);
		auto de = new PGUPV::DrawElements(GL_LINE_STRIP, 17, GL_UNSIGNED_SHORT, nullptr);
		de->setRestartIndex(0xFFFF);
		de->setPrimitiveRestart();

		mesh->addDrawCommand(de);

		frustum = std::make_shared<Model>();
		frustum->addMesh(mesh);

		Camera::axes = std::make_unique<Axes>();
	}

	if (recomputeFrustum) {
		updateVerticesInFrustum();
		recomputeFrustum = false;
	}

	auto prev = PGUPV::ConstantIllumProgram::use();

	auto bo = gl_uniform_buffer.getBound(UBO_GL_MATRICES_BINDING_INDEX);
	auto mats = std::static_pointer_cast<GLMatrices>(bo);
	if (mats) {
		mats->pushMatrix(GLMatrices::MODEL_MATRIX);
		mats->loadIdentity(GLMatrices::MODEL_MATRIX);

		mats->translate(GLMatrices::MODEL_MATRIX, getCameraPos());

		mat4 rot = glm::mat4(
			glm::vec4(getRightDirection(), 0.0f),
			glm::vec4(getUpDirection(), 0.0f),
			glm::vec4(-getViewDirection(), 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		mats->multMatrix(GLMatrices::MODEL_MATRIX, rot);
		mats->pushMatrix(GLMatrices::MODEL_MATRIX);
		mats->scale(GLMatrices::MODEL_MATRIX, axesScale);
		axes->render();
		mats->popMatrix(GLMatrices::MODEL_MATRIX);
	}


	frustum->render();
	if (prev) prev->use();

	if (mats)
		mats->popMatrix(PGUPV::GLMatrices::MODEL_MATRIX);
}

void Camera::setFrustumColor(const glm::vec4 & color)
{
	frustumColor = color;
	if (frustum) {
		auto& mesh = frustum->getMesh(0);
		mesh.setColor(color);
	}
}
