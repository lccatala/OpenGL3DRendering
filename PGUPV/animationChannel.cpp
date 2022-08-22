#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "log.h"

#include "animationChannel.h"

using PGUPV::AnimationChannel;
using PGUPV::KeyFrameValue;

AnimationChannel::AnimationChannel(const std::string &name)
	: nodeName(name)
{
}

std::string AnimationChannel::getNodeName() const {
	return nodeName;
}

void AnimationChannel::addPositionKeyFrame(const KeyFrameValue<glm::vec3>& pos)
{
	positions.push_back(pos);
}

void AnimationChannel::addRotationKeyFrame(const KeyFrameValue<glm::quat>& rot)
{
	rotations.push_back(rot);
}

void AnimationChannel::addScalingKeyFrame(const KeyFrameValue<glm::vec3>& sca)
{
	scalings.push_back(sca);
}

uint32_t AnimationChannel::getNumFrames() const
{
	return static_cast<uint32_t>(std::max({ positions.size(), scalings.size(), rotations.size() }));
}

template <typename T>
T linearInterpolation(const std::vector<KeyFrameValue<T>> &keyframes, 
	float t, std::function<T (const T &x, const T &y, float a)> lerpFunc, 
	const T &identity) {
	if (keyframes.empty()) {
		return identity;
	}

	if (t <= keyframes[0].tick) {
		return keyframes[0].value;
	}
	else if (t >= keyframes.back().tick) {
		return keyframes.back().value;
	}
	uint32_t i = 0;
	while (t > keyframes[i].tick) {
		i++;
	}

	// keyframes[i-1].tick < t <= keyframes[i].tick 
	auto start = keyframes[i - 1];
	auto end = keyframes[i];

//	return (t - start.tick)*(end.value - start.value) / static_cast<float>(end.tick - start.tick) + start.value;
	return lerpFunc(start.value, end.value, (t - start.tick) / static_cast<float>(end.tick - start.tick));
}

glm::vec3 AnimationChannel::interpolatePosition(float t) const
{
	return linearInterpolation<glm::vec3>(positions, t, [](const glm::vec3 &x, const glm::vec3 &y, float t) {
		return glm::mix(x, y, t);
	}, glm::vec3(0.0f));
}

glm::quat AnimationChannel::interpolateRotation(float t) const 
{
	return linearInterpolation<glm::quat>(rotations, t, [](const glm::quat &x, const glm::quat &y, float t) {
		return glm::slerp(x, y, t);
	}, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
}

glm::vec3 AnimationChannel::interpolateScaling(float t) const 
{
	return linearInterpolation<glm::vec3>(scalings, t, [](const glm::vec3 &x, const glm::vec3 &y, float t) {
		return glm::mix(x, y, t);
	}, glm::vec3(1.0f));
}

glm::mat4 AnimationChannel::interpolate(float t) const
{
	return 
		glm::translate(glm::mat4(1.0f), interpolatePosition(t)) *
		glm::mat4_cast(interpolateRotation(t)) *
		glm::scale(glm::mat4(1.0f), interpolateScaling(t));
}
