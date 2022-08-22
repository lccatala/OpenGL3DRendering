#pragma once

#include <string>
#include <vector>

#include <glm/gtc/quaternion.hpp>

#include "value.h"

namespace PGUPV {
	template <typename T>
	struct KeyFrameValue {
		float tick;
		T value;
	};


	class AnimationChannel {
	public:
		AnimationChannel(const std::string &name);
		std::string getNodeName() const;
		void addPositionKeyFrame(const KeyFrameValue<glm::vec3> &pos);
		void addRotationKeyFrame(const KeyFrameValue<glm::quat> &rot);
		void addScalingKeyFrame(const KeyFrameValue<glm::vec3> &sca);

		uint32_t getNumFrames() const;

		/**
		Return the interpolated position at t 
		\param t time point to interpolate (in ticks)
		\return the interpolated position
		*/
		glm::vec3 interpolatePosition(float t) const;
		/**
		Return the interpolated rotation at t 
		\param t time point to interpolate (in ticks)
		\return the interpolated rotation
		*/
		glm::quat interpolateRotation(float t) const;
		/**
		Return the interpolated scaling at t
		\param t time point to interpolate (in ticks)
		\return the interpolated scaling
		*/
		glm::vec3 interpolateScaling(float t) const;
		/**
		Return the interpolated transformation at t 
		\param t time point to interpolate (in ticks)
		\return the interpolated traformation
		*/
		glm::mat4 interpolate(float t) const;
	private:
		std::string nodeName;
		std::vector<KeyFrameValue<glm::vec3>> positions;
		std::vector<KeyFrameValue<glm::quat>> rotations;
		std::vector<KeyFrameValue<glm::vec3>> scalings;
	};
};
