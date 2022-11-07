#pragma once

#include "gl_incs.h"
#include "Renderable.h"
#include "State.h"

#include <optional>

class Shape
{
public:
  Shape(Accessor<State> state_acc) : state_acc_(state_acc) {}

public:
  glm::vec3 get_pos() const;
  virtual void set_pos(const glm::vec3 &pos);
  glm::vec3 get_vel() const;
  virtual void set_vel(const glm::vec3 &v);

public:
  void add_renderable(Renderable r);
  void update_model_if_renderable(const glm::quat &orientation, const glm::vec3 &dims);

private:
  std::optional<Renderable> r_;
  Accessor<State> state_acc_;
};
