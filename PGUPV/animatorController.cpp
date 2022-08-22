#include "animatorController.h"
#include "group.h"
#include "nodeVisitor.h"
#include "transform.h"
#include "animationClip.h"
#include "skeleton.h"
#include "bone.h"

using PGUPV::AnimatorState;
using PGUPV::AnimatorController;
using PGUPV::AnimationClip;
using PGUPV::Group;


void AnimatorState::setAnimationClip(std::shared_ptr<AnimationClip> clip) {
	animationClip = clip;
}

void AnimatorState::setSpeed(float speed) {
	animationSpeed = speed;
}

void AnimatorState::update(uint32_t ms)
{

	animationTime += static_cast<uint32_t>(ms * animationSpeed + 0.5f);
}

bool AnimatorState::interpolate(const std::string & boneId, glm::mat4 & mat) const
{
	if (animationClip) {
		return animationClip->interpolate(animationTime / 1000.f, boneId, mat);
	}
	return false;
}

void AnimatorState::reset()
{
	animationTime = 0;
}

AnimatorController::AnimatorController(const std::string & name) : 
	animationControllerId(name), status(Status::Stopped)
{
}

void AnimatorController::setSubScene(std::shared_ptr<Group> root)
{
	scene = root;
}

void AnimatorController::addState(std::shared_ptr<AnimatorState> state)
{
	currentAnimationState = state;
}

void AnimatorController::update(uint32_t ms) {
	if (status == Status::Playing)
		currentAnimationState->update(ms);
}

void AnimatorController::start()
{
	if (status != Status::Paused)
		currentAnimationState->reset();
	status = Status::Playing;
}

void PGUPV::AnimatorController::stop()
{
	currentAnimationState->reset();
	status = Status::Stopped;
}

void AnimatorController::pause()
{
	status = Status::Paused;
}
