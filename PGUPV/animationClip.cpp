#include "animationClip.h"
#include "animationChannel.h"
#include "utils.h"

#include <cmath>

using PGUPV::AnimationClip;
using PGUPV::AnimationChannel;

AnimationClip::AnimationClip(const std::string & name, float durationInTicks, float ticksPerSecond)
	: id(name), totalTicks(durationInTicks), ticksPerSec(ticksPerSecond),
	wrapMode(WrapMode::ONCE)
{
}

void AnimationClip::addChannel(std::shared_ptr<AnimationChannel> channel)
{
	channels[channel->getNodeName()] = channel;
}

std::string AnimationClip::getName() const {
	return id;
}

void AnimationClip::setName(const std::string &name) {
	id = name;
}


void AnimationClip::setDurationInTicks(float ticks)
{
	totalTicks = ticks;
}

float AnimationClip::getDurationInTicks() const {
	return totalTicks;
}

void AnimationClip::setTicksPerSecond(float tps)
{
	ticksPerSec = tps;
}

float AnimationClip::getTicksPerSecond() const {
	return ticksPerSec;
}

float AnimationClip::getDurationInSeconds() const {
	return static_cast<float>(totalTicks) / ticksPerSec;
}

uint32_t AnimationClip::getNumChannels() const {
	return static_cast<uint32_t>(channels.size());
}

void AnimationClip::setWrapMode(WrapMode mode)
{
	wrapMode = mode;
}

const std::vector<std::shared_ptr<AnimationChannel>> AnimationClip::getAnimationChannels() const {
	return PGUPV::getValuesFromMap(channels);
}

float wrapAnimationTime(float t, AnimationClip::WrapMode wrapMode, float durationInTicks) {
	switch (wrapMode) {
	case AnimationClip::WrapMode::ONCE:
		if (t > durationInTicks) return 0.0f;
		return t;
		break;
	case AnimationClip::WrapMode::PING_PONG:
		t = std::fmod(t, 2 * durationInTicks);
		if (t < durationInTicks)
			return t;
		return 2 * durationInTicks - t;
		break;
	case AnimationClip::WrapMode::CLAMP_FOREVER:
		if (t > durationInTicks) return durationInTicks;
		return t;
		break;
	case AnimationClip::WrapMode::LOOP:
		return std::fmod(t, durationInTicks);
		break;
	default:
		ERRT("Modo de repetición no válido");
	}
}


bool AnimationClip::interpolate(const float t, const std::string & boneId, glm::mat4 & mat) const
{
	float theTime = wrapAnimationTime(t * ticksPerSec, wrapMode, getDurationInTicks());
	auto ac = getAnimationChannel(boneId);
	if (ac) {
		mat = ac->interpolate(theTime);
		return true;
	}
	return false;
}

const std::shared_ptr<AnimationChannel> AnimationClip::getAnimationChannel(const std::string &name) const {
	const auto ac = channels.find(name);
	if (ac != channels.end()) {
		return ac->second;
	}
	return std::shared_ptr<AnimationChannel>();
}


