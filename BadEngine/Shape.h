#pragma once

#include "gl_incs.h"
#include "Renderable.h"

#include <optional>

class Shape
{
public:
  Shape(Accessor<glm::vec3> pos_acc) : pos_acc_(pos_acc) {}

public:
  glm::vec3 get_pos() const;
  virtual void set_pos(const glm::vec3 &pos);

public:
  void add_renderable(Renderable r);
  void render(const glm::quat &orientation, const glm::vec3 &dims);

private:
  std::optional<Renderable> r_;
  Accessor<glm::vec3> pos_acc_;
};
