#pragma once

#include <glm/fwd.hpp>
#include <memory>

namespace PGUPV {
	class Texture2D;
	void texture2DBlit(const glm::vec3 &pos, float height, std::shared_ptr<Texture2D> texture);
};