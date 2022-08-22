#pragma once

#include <string>
#include <memory>
#include <glm/fwd.hpp>

namespace PGUPV {
	class AnimationClip;
	class AnimatorState {
	public:
		AnimatorState(const std::string &name) : stateName(name), animationSpeed(1.0f), animationTime(0) {};
		void setAnimationClip(std::shared_ptr<AnimationClip> clip);
		std::shared_ptr<AnimationClip> getAnimationClip() const { return animationClip; }
		float getSpeed() const { return animationSpeed; }
		void setSpeed(float speed);
		void update(uint32_t ms);

		/**
		Calcula la transformación interpolada en el instante dado para el hueso indicado
		\param boneId hueso del que se desea calcular la interpolación
		\param mat [out] matriz donde escribir la interpolación, si existe el hueso
		\return true si el clip de animación tiene datos para el hueso indicado, o false en otro caso
		*/
		bool interpolate(const std::string &boneId, glm::mat4 &mat) const;
		void reset();
	private:
		std::string stateName;
		std::shared_ptr<AnimationClip> animationClip;
		float animationSpeed;
		uint64_t animationTime;
	};

	class Group;

	class AnimatorController {
	public:
		AnimatorController(const std::string &name);
		void setSubScene(std::shared_ptr<Group> root);
		void addState(std::shared_ptr<AnimatorState> state);
		std::shared_ptr<AnimatorState> currentState() const {
			return currentAnimationState;
		}
		void update(uint32_t ms);
		void start();
		void stop();
		void pause();
		enum class Status { Playing, Paused, Stopped };
		Status getStatus() const { return status; }
	private:
		std::string animationControllerId;
		std::shared_ptr<AnimatorState> currentAnimationState;
		Status status;
		std::shared_ptr<Group> scene;
	};
};
