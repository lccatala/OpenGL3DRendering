
#include <glm/glm.hpp>

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4127)
#endif 

#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
#pragma warning (pop)
#endif


#include "json.hpp"
#include "camerahandler.h"
#include "app.h"
#include "window.h"
#include "log.h"
#include "utils.h"

using json = nlohmann::json;

using PGUPV::App;
using PGUPV::Camera;
using PGUPV::CameraHandler;
using PGUPV::OrbitCameraHandler;
using PGUPV::WalkCameraHandler;
using PGUPV::XYPanZoomCamera;
using PGUPV::MouseMotionEvent;
using PGUPV::MouseWheelEvent;

using glm::vec3;
using glm::vec4;
using glm::mat4;

#define HALF_PI glm::half_pi<float>()
#define PI glm::pi<float>()
#define TWO_PI glm::two_pi<float>()

const Camera &CameraHandler::getCamera() {
	return *camera;
}

void CameraHandler::resized(uint width, uint height) {
	if (!camera) {
		WARN("CameraHandler sin cámara");
	}
	else {
		if (height == 0) height = 1;
		float ar = (float)width / height;
		if (std::isnan(camera->getNear())) {
			// default camera (identity projection matrix)
			camera->setProjMatrix(glm::perspective(glm::radians(60.0f), ar, 0.1f, 100.f));
		}
		else {
			float fov, near, far;
				fov = camera->getFOV();
				near = camera->getNear();
				far = camera->getFar();
				camera->setProjMatrix(glm::perspective(fov, ar, near, far));
		}
	}
}

#define WHEEL_SCALE 50.0f


OrbitCameraHandler::OrbitCameraHandler(float distanceToOrigin,
	float initLongitude,
	float initLatitude) {
	_initd = distanceToOrigin;
	_initlong = initLongitude;
	_initlat = initLatitude;
	_initcenter = vec3(0.0f, 0.0f, 0.0f);
	resetView();
}

void OrbitCameraHandler::resetView() {
	_d = _initd;
	_long = _initlong;
	_lat = _initlat;
	_center = _initcenter;
	updateMatrix();
}

bool OrbitCameraHandler::mouse_move(const MouseMotionEvent &e) {

	if (e.state) {
		uint width = e.wsrc->width();
		// Botón izquierdo: rotar
		if (e.state & 1)
			moveRelative(0.0, (float)-e.xrel / width, (float)e.yrel / width);
		else
			// Botón derecho: acercar/alejar la cámara
			if (e.state & 4)
				moveRelative(-(float)e.yrel / width, 0.0f, 0.0f);
			else
				// Botón central: pan
				if (e.state & 2)
					this->pan(-(float)e.xrel / width, (float)e.yrel / width);
		return true;
	}
	return false;
}

bool OrbitCameraHandler::mouse_wheel(const MouseWheelEvent &e) {
	float scale = WHEEL_SCALE;
	if (App::isKeyPressed(PGUPV::KeyCode::LeftShift)) {
		scale *= 10.0f;
	}
	moveRelative((float)e.y * scale / e.wsrc->width(), 0.0f, 0.0f);
	return true;
}

void OrbitCameraHandler::move(float distance, float longitude, float latitude) {
	_d = distance;
	_long = longitude;
	_lat = latitude;
	updateMatrix();
}

void OrbitCameraHandler::pan(float dright, float dup) {

	double coslat = cos(_lat);
	// pos: position with respect to the origin (distance to the origin = 1)
	vec3 pos = vec3(coslat * sin(_long), sin(_lat),
		coslat * cos(_long));
	// v: direction from the origin towards the camera
	vec3 v = -pos;
	// v = v/glm::length(v);
	vec3 R = vec3(0.0, 1.0, 0.0) - v * v.y;
	vec3 u = glm::normalize(R); // /glm::length(R);

	vec3 r = glm::cross(v, u);
	_center = _center + dright * 10.f * r + dup * 10.f * u;
	//INFO("Pan: x=" + std::to_string(dright) + " y=" + std::to_string(dup));
	updateMatrix();
}

void OrbitCameraHandler::moveRelative(float ddelta, float longDelta,
	float latDelta) {
	_d += ddelta;
	if (_d < 0.0f)
		_d = 0.1f;

	_long += longDelta * TWO_PI;
	if (_long < 0.0f)
		_long += TWO_PI;
	else if (_long >= TWO_PI)
		_long -= TWO_PI;

	_lat += latDelta * TWO_PI;
	if (_lat < -HALF_PI)
		_lat = -HALF_PI;
	else if (_lat > HALF_PI)
		_lat = HALF_PI;

	updateMatrix();
}

void OrbitCameraHandler::setPointOfInterest(const glm::vec3 & poi)
{
	_center = poi;
	updateMatrix();
}


void OrbitCameraHandler::setProjMatrix(const glm::mat4 &matrix) {
	camera->setProjMatrix(matrix);
}


void OrbitCameraHandler::updateMatrix() {
	glm::mat4 view = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -_d));
	view = glm::rotate(view, _lat, vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, -_long, vec3(0.0f, 1.0f, 0.0f));
	view = glm::translate(view, -_center);
	//glm::vec3 pos = _center + _d * glm::vec3{ cosf(_lat)*sinf(_long), sinf(_lat), cosf(_lat)*cosf(_long) };
	//glm::mat4 view = glm::lookAt(pos, _center, glm::vec3{ 0.0f, 1.0f, 0.0f });
	camera->setViewMatrix(view);
}

bool OrbitCameraHandler::keyboard(const PGUPV::KeyboardEvent &e) {
	if (e.key == KeyCode::Space) {
		if (e.state == ButtonState::Pressed)
			resetView();
		return true;
	}
	else
		return false;
}

/**
Guarda el estado actual de la cámara en el flujo indicado
*/
void OrbitCameraHandler::saveStatus(std::ostream &stream) {
	json j;
	j["orbitcamera"] = true;
	j["long"] = _long;
	j["lat"] = _lat;
	j["distance"] = _d;
	j["centerX"] = _center.x; j["centerY"] = _center.y; j["centerZ"] = _center.z;
	j["initlong"] = _initlong;
	j["initlat"] = _initlat;
	j["initdistance"] = _initd;
	j["initcenterX"] = _initcenter.x;   j["initcenterY"] = _initcenter.y;   j["initcenterZ"] = _initcenter.z;
	stream << j.dump() << std::endl;
}

static json readJsonFromFile(std::istream &stream) {
	json j;
	std::string str;
	if (j.empty()) {
		try
		{
			std::getline(stream, str);
			if (stream.eof()) ERR("No se ha podido abrir el fichero para leer el estado de la cámara");
			j = json::parse(str);
		}
		catch (const std::exception&e)
		{
			ERRT(std::string("Error leyendo el fichero del estado de la cámara: ") + e.what());
		}
	}

	return j;
}

/**
restaura el estado de la cámara al que se encuentra en el flujo
*/
void OrbitCameraHandler::loadStatus(std::istream &stream) {
	json j = readJsonFromFile(stream);

	if (!j["orbitcamera"]) {
		ERR("Tipo de cámara incompatible. Usando OrbitCameraHandler");
		return;
	}

	_long = j["long"];
	_lat = j["lat"];
	_d = j["distance"];
	_center.x = j["centerX"]; _center.y = j["centerY"]; _center.z = j["centerZ"];
	_initlong = j["initlong"];
	_initlat = j["initlat"];
	_initd = j["initdistance"];
	_initcenter.x = j["initcenterX"]; _initcenter.y = j["initcenterY"]; _initcenter.z = j["initcenterZ"];

	updateMatrix();
}



XYPanZoomCamera::XYPanZoomCamera(float width, glm::vec2 center)
	: _initWidth(width), _aspectRatio(1.0), _initCenter(center), _wheel_speed(5.0f) {
	_width = width;
	_center = center;
	updateView();
	updateProj();
}

void XYPanZoomCamera::updateProj() {
	float cameraHeight = _width / _aspectRatio;
	camera->setProjMatrix(glm::ortho(-_width / 2.0f, _width / 2.0f, -cameraHeight / 2.0f,
		cameraHeight / 2.0f, 0.0f, 100.0f));
}

void XYPanZoomCamera::updateView() {
	camera->setViewMatrix(glm::lookAt(vec3(_center, DISTANCE_TO_PLANE),
		vec3(_center, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
}

bool XYPanZoomCamera::mouse_move(const MouseMotionEvent &e) {
	if (e.state) {
		uint width = e.wsrc->width();
		// Botón izquierdo: pan
		if (e.state & 1) {
			_center -= glm::vec2(e.xrel * _width / width,
				-(e.yrel * _width) / (width * _aspectRatio));
			updateView();
		}
		else
			// Botón derecho: acercar/alejar la cámara
			if (e.state & 4) {
				_width = _width - e.yrel * _width / (width * _aspectRatio);
				updateProj();
			}

		return true;
	}
	return false;
}

bool XYPanZoomCamera::mouse_wheel(const MouseWheelEvent &e) {
	_width = _width + (float)e.y * _width * _wheel_speed / e.wsrc->width();
	updateProj();
	return true;
}

void XYPanZoomCamera::resetView() {
	_width = _initWidth;
	_center = _initCenter;
}

void XYPanZoomCamera::resized(uint width, uint height) {
	if (height == 0)
		height = 1;
	_aspectRatio = (float)width / height;
	updateProj();
}


void XYPanZoomCamera::saveStatus(std::ostream &stream) {

	json j;
	j["xypanzoomcamera"] = true;
	j["width"] = _width;
	j["centerX"] = _center.x; j["centerY"] = _center.y;
	j["aspectratio"] = _aspectRatio;
	j["wheelspeed"] = _wheel_speed;
	j["initwidth"] = _initWidth;
	j["initcenterX"] = _initCenter.x;   j["initcenterY"] = _initCenter.y;
	stream << j.dump() << std::endl;
}


void XYPanZoomCamera::loadStatus(std::istream &stream) {
	json j = readJsonFromFile(stream);

	if (!j["xypanzoomcamera"]) {
		ERR("Tipo de cámara incompatible. Usando ZYPanZoomCamera");
		return;
	}
	_width = j["width"];
	_center.x = j["centerX"]; _center.y = j["centerY"];
	_aspectRatio = j["aspectratio"];
	_wheel_speed = j["wheelspeed"];
	_initWidth = j["initwidth"];
	_initCenter.x = j["initcenterX"];
	_initCenter.y = j["initcenterY"];

	updateView();
	updateProj();
}




WalkCameraHandler::WalkCameraHandler(float h, const glm::vec4 &pos, float yaw,
	float pitch) {
	_inith = h;
	_initpos = pos;
	_inityaw = yaw;
	_initpitch = pitch;

	_deltah = 0.1f;
	_walkSpeed = 1.0f;
	_deltayaw = _deltapitch = 1.8f;

	captureMouse();
	resetView();
}

WalkCameraHandler::~WalkCameraHandler() { releaseMouse(); }

void WalkCameraHandler::releaseMouse() {
	mouseCaptured = false;
	App::getInstance().getWindow().showMouseCursor(true);
}

void WalkCameraHandler::captureMouse() {
	Window &w = App::getInstance().getWindow();

	w.showMouseCursor(false);
	w.setMousePosition(w.width() / 2, w.height() / 2);
	mouseCaptured = true;
}

bool WalkCameraHandler::isMouseCaptured() { return mouseCaptured; }

void WalkCameraHandler::resetView() {
	_h = _inith;
	_pos = _initpos;
	_yaw = _inityaw;
	_pitch = _initpitch;
	updateMatrix();
}

bool WalkCameraHandler::mouse_wheel(const MouseWheelEvent &e) {

	_pos.y -= _deltah * e.y / 2.0f; /// 120.f;
	updateMatrix();
	return true;
}

bool WalkCameraHandler::mouse_move(const MouseMotionEvent &e) {

	if (!mouseCaptured)
		return true;

	Window &theWindow = App::getInstance().getWindow();
	int halfwidth = theWindow.width() / 2;
	int halfheight = theWindow.height() / 2;

	int xrel, yrel;

	if (halfwidth == e.x && halfheight == e.y)
		return true;

	xrel = e.x - halfwidth;
	yrel = e.y - halfheight;

	_yaw -= (xrel * PI) / (_deltayaw * halfwidth);
	_pitch -= (yrel * PI) / (_deltapitch * halfheight);
	if (_pitch < -HALF_PI)
		_pitch = -HALF_PI;
	else if (_pitch > HALF_PI)
		_pitch = HALF_PI;

	updateMatrix();

	theWindow.setMousePosition(halfwidth, halfheight);
	return true;
}

void WalkCameraHandler::updateMatrix() {
	mat4 m(glm::rotate(glm::mat4(1.0f), -_pitch, vec3(1.0f, 0.0f, 0.0f)));
	m = glm::rotate(m, -_yaw, vec3(0.0f, 1.0f, 0.0f));
	camera->setViewMatrix(glm::translate(m, -vec3(_pos)));
}



void WalkCameraHandler::advance(float delta) {
	// Avanzar o retroceder
	vec4 ahead = vec4(0.f, 0.f, -1.f, 1.f);
	ahead = glm::rotate(glm::mat4(1.0f), _yaw, vec3(0.0f, 1.0f, 0.0f)) * ahead;
	ahead = ahead * (_walkSpeed * delta / 1000.0f);
	_pos = _pos + ahead;
}

void WalkCameraHandler::update(uint ms) {
	bool forwardPressed = App::isKeyPressed(PGUPV::KeyCode::W);
	bool backwardPressed = App::isKeyPressed(PGUPV::KeyCode::S);
	bool leftPressed = App::isKeyPressed(PGUPV::KeyCode::A);
	bool rightPressed = App::isKeyPressed(PGUPV::KeyCode::D);

	if ((forwardPressed || backwardPressed) &&
		!(forwardPressed && backwardPressed)) {
		if (forwardPressed)
			advance(static_cast<float>(ms));
		else
			advance(-static_cast<float>(ms));
	}
	if ((leftPressed || rightPressed) &&
		!(leftPressed && rightPressed)) {
		// Pasos laterales
		vec4 ahead = vec4(0.f, 0.f, -1.f, 1.f);
		if (leftPressed)
			ahead = glm::rotate(glm::mat4(1.0f), _yaw + glm::radians(90.0f), vec3(0.f, 1.f, 0.f)) * ahead;
		else
			ahead = glm::rotate(glm::mat4(1.0f), _yaw - glm::radians(90.0f), vec3(0.f, 1.f, 0.f)) * ahead;
		ahead = ahead * (_walkSpeed * ms / 1000.0f);
		_pos = _pos + ahead;
	}

	updateMatrix();
}

// Establece la altura de la cámara
void WalkCameraHandler::setHeight(float height) {
	_h = height;
	updateMatrix();
}
// Establece la posición de la cámara
void WalkCameraHandler::setPos(const glm::vec4 &pos) {
	_pos = pos;
	updateMatrix();
}

// Establece los ángulos de la dirección de la vista (ver definición
// en el constructor
void WalkCameraHandler::setAngles(float pitch, float yaw) {
	_pitch = pitch;
	_yaw = yaw;
	updateMatrix();
}

void WalkCameraHandler::setSpeeds(float deltah, float deltayaw,
	float deltapitch) {
	_deltah = deltah;
	_deltayaw = deltayaw;
	_deltapitch = deltapitch;
}


/**
Guarda el estado actual de la cámara en el flujo indicado
*/
void WalkCameraHandler::saveStatus(std::ostream &stream) {

	json j;
	j["walkcamera"] = true;

	j["yaw"] = _yaw;
	j["pitch"] = _pitch;
	j["height"] = _h;
	j["deltayaw"] = _deltayaw;
	j["deltapitch"] = _deltapitch;
	j["deltaheight"] = _deltah;
	j["posX"] = _pos.x; j["posY"] = _pos.y; j["posZ"] = _pos.z;

	j["walkspeed"] = _walkSpeed;

	j["inityaw"] = _inityaw;
	j["initpitch"] = _initpitch;
	j["initheight"] = _inith;
	j["initposX"] = _initpos.x; j["initposY"] = _initpos.y; j["initposZ"] = _initpos.z;
	stream << j.dump() << std::endl;
}


/**
restaura el estado de la cámara al que se encuentra en el flujo
*/
void WalkCameraHandler::loadStatus(std::istream &stream) {
	json j = readJsonFromFile(stream);

	if (!j["walkcamera"]) {
		ERR("Tipo de cámara incompatible. Usando WalkCamera");
		return;
	}
	_yaw = j["yaw"];
	_pitch = j["pitch"];
	_h = j["height"];
	_deltayaw = j["deltayaw"];
	_deltapitch = j["deltapitch"];
	_deltah = j["deltaheight"];
	_pos.x = j["posX"]; _pos.y = j["posY"]; _pos.z = j["posZ"];

	_walkSpeed = j["walkspeed"];

	_inityaw = j["inityaw"];
	_initpitch = j["initpitch"];
	_inith = j["initheight"];
	_initpos.x = j["initposX"]; _initpos.y = j["initposY"]; _initpos.z = j["initposZ"];

	updateMatrix();
}