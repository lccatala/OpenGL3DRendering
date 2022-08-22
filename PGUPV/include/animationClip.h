#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <glm/fwd.hpp>

namespace PGUPV {
	class AnimationChannel;

	class AnimationClip {
	public:
		AnimationClip(const std::string &name, float durationInTicks, float ticksPerSecond);
		void addChannel(std::shared_ptr<AnimationChannel> channel);
		std::string getName() const;
		void setName(const std::string &name);
		void setDurationInTicks(float ticks);
		float getDurationInTicks() const;
		void setTicksPerSecond(float tps);
		float getTicksPerSecond() const;
		float getDurationInSeconds() const;
		uint32_t getNumChannels() const;

		/**
		Modos de reproducción:
		ONCE: reproducir la animación una única vez (y vuelve a la posición inicial)
		LOOP: repetir la animación indefinidamente
		PING_PONG: reproducir la animación, al acabar reproducir en sentido contrario y repetir
		CLAMP_FOREVER: reproducir la animación una única vez (y quedarse en la última posición)
		*/
		enum class WrapMode {ONCE, LOOP, PING_PONG, CLAMP_FOREVER};
		void setWrapMode(WrapMode mode);
		WrapMode getWrapMode() const { return wrapMode; }
		
		/**
		Calcula la transformación interpolada en el instante dado para el hueso indicado
		\param t instante de la animación que se desea interpolar, en segundos. Se tiene en cuenta wrapMode
		\param boneId hueso del que se desea calcular la interpolación
		\param mat [out] matriz donde escribir la interpolación, si existe el hueso
		\return true si el clip de animación tiene datos para el hueso indicado, o false en otro caso
		*/
		bool interpolate(const float t, const std::string &boneId, glm::mat4 &mat) const;

		const std::shared_ptr<AnimationChannel> getAnimationChannel(const std::string &name) const;
		const std::vector<std::shared_ptr<AnimationChannel>> getAnimationChannels() const;
	private:
		std::string id;
		float totalTicks, ticksPerSec;
		std::map<std::string, std::shared_ptr<AnimationChannel>> channels;
		WrapMode wrapMode;
	};
};
