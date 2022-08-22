#pragma once
// 2016
#include <memory>
#include "nodeVisitor.h"
#include "stockModels.h"

/**
\class RenderBoundingVolumes
Clase usada para dibujar los volúmenes de inclusión de una escena.
Es un ejemplo de uso del patrón del visitante
*/

namespace PGUPV {
	class Group;
	class Transform;
	class Geode;

	class RenderBoundingVolumes : public NodeVisitor {
	public:
		enum class Volume { BOX, SPHERE };
		RenderBoundingVolumes(Volume volume = Volume::BOX) :
			lastColor(0), volume(volume), current(1.0f) {};
		void apply(Group &group) override;
		void apply(Transform &transform) override;
		void apply(Geode &geode) override;
		void reset();
		void showVolume(Volume vol) { volume = vol; }
	private:
		void renderBBox(const BoundingBox &bb, bool highlight, const glm::vec4 &color);
		void renderBSph(const BoundingSphere &bb, bool highlight, const glm::vec4 &color);
		unsigned char lastColor;
		Volume volume;
		glm::mat4 current;
		static std::unique_ptr<WireBox> box;
		static std::unique_ptr<Sphere> sphere;
	};
};