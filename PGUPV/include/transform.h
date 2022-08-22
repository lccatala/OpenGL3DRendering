#pragma once
// 2016

#include <memory>
#include "group.h"
#include "value.h"


namespace PGUPV {
  class Transform : public Group {
  public:
    static std::shared_ptr<Transform> build(const glm::mat4 &xform = glm::mat4(1.0f));
    void setTransform(const glm::mat4 &xform);
    glm::mat4 getTransform() const;
    Value<glm::mat4> &getValue();
    void render() override;
    std::shared_ptr<Transform> shared_from_this();
    void accept(NodeVisitor &visitor) override;
  protected:
    Transform(const glm::mat4 &xform = glm::mat4(1.0f));
    Value<glm::mat4> transf;
    void recomputeBoundingBox() override;
    void recomputeBoundingSphere() override;
  };
};
