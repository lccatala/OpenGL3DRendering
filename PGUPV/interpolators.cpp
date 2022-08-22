
#include <glm/glm.hpp>

#include "interpolators.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


using PGUPV::CircularInterpolator;
using PGUPV::LinearInterpolator;
using PGUPV::MultiLinearInterpolator;


CircularInterpolator::CircularInterpolator(glm::vec4 center, glm::vec4 initpos, 
	glm::vec3 axis, float time_cycle, float radians_cycle, int type) {
	_center = center;
	_initpos = initpos;
	_initpos2 = initpos - center;
	_axis = axis;
	_time_cycle = time_cycle;
	_radians_cycle = radians_cycle;
	_type = type;

	_endpoint = glm::rotate(glm::mat4(1.0f), radians_cycle, axis) * _initpos2 + center;
}



glm::vec4 CircularInterpolator::interpolate(double t) {
	if (t <= 0.0) return _initpos;
	if (t >= _time_cycle) {
		if (_type == ONE_TIME) return _endpoint;
		if (_type == REPEAT) t = fmod(t, (double)_time_cycle);
		else {
			t = fmod(t, 2.0 * _time_cycle);
			if (t >= _time_cycle) t = 2 * _time_cycle - t;
		}
	}
	return glm::rotate(glm::mat4(1.0f), 
		(float)(_radians_cycle * t / _time_cycle), _axis) * _initpos2 + _center;
}

