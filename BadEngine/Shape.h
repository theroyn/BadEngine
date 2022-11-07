#pragma once

#include "gl_incs.h"
#include "Renderable.h"
#include "Collidable.h"
#include "State.h"

#include <optional>

class Shape
{
public:
  Shape(Accessor<State> state_acc) : state_acc_(state_acc) {}

public:
  glm::vec3 get_pos() const;
  virtual void set_pos(const glm::vec3 &pos);
  glm::quat get_orientation() const;
  virtual void set_orientation(const glm::quat &q);
  glm::vec3 get_vel() const;
  virtual void set_vel(const glm::vec3 &v);
  glm::vec3 get_angular_vel() const;
  virtual void set_angular_vel(const glm::vec3 &w);

public:
  void add_renderable(Renderable r);
  void add_collidable(float mass);
  Collidable &get_collidable();
  Collidable get_collidable() const;
  bool has_collidable() const;
  void update_model_if_renderable(const glm::vec3 &dims);

private:
  virtual Collidable create_collidable(float mass) const = 0;

private:
  std::optional<Renderable> r_;
  std::optional<Collidable> c_;
  Accessor<State> state_acc_;
};
