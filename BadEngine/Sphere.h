#pragma once

#include "gl_incs.h"
#include "Renderable.h"
#include "Accessor.h"
#include "Shape.h"

#include <mutex>
#include <optional>
#include <unordered_set>

class Sphere : public Shape
{
public:
  Sphere(float x,
         float y,
         float z,
         float rad,
         Accessor<glm::vec3> pos_acc,
         Accessor<glm::vec3> vel_acc) : Shape(pos_acc),
                                        mass(7.f),
                                        rad(rad),
                                        elasticity(.9f),
                                        vel_acc_(vel_acc)
  {
    set_pos(glm::vec3(x, y, z));
    set_vel(glm::vec3(0.f));
  }

  glm::vec3 get_vel() const { return vel_acc_.get(); }

  void set_vel(const glm::vec3 &v)
  {
    vel_acc_.set(v);
  }

  void set_pos(const glm::vec3 &pos) override
  {
    Shape::set_pos(pos);
    render(glm::identity<glm::quat>(), glm::vec3(rad)); // DUDU identity orientation
  }

  float rad;
  float elasticity;
  float mass;
  std::mutex in_collision_m_;
  std::unordered_set<Sphere *> colliders_;

private:
  Accessor<glm::vec3> vel_acc_;
};
