#pragma once

#include "gl_incs.h"
#include "Renderable.h"
#include "Accessor.h"

#include <mutex>
#include <optional>
#include <unordered_set>

struct Sphere
{
  Sphere(float x,
         float y,
         float z,
         float rad,
         Accessor<glm::vec3> pos_acc,
         Accessor<glm::vec3> vel_acc) : mass(7.f),
                                        rad(rad),
                                        elasticity(.9f),
                                        pos_acc_(pos_acc),
                                        vel_acc_(vel_acc)
  {
    set_pos(glm::vec3(x, y, z));
    set_vel(glm::vec3(0.f));
  }

  void add_renderable(Renderable r)
  {
    r_ = r;
  }

  glm::vec3 get_vel() const { return vel_acc_.get(); }

  void set_vel(const glm::vec3 &v)
  {
    vel_acc_.set(v);
  }

  glm::vec3 get_pos() const { return pos_acc_.get(); }

  void set_pos(const glm::vec3 &pos)
  {
    pos_acc_.set(pos);
    if (r_.has_value())
    {
      glm::mat4 model_trans(1.f);

      model_trans = glm::translate(model_trans,
                                   glm::vec3(pos));
      model_trans = glm::scale(model_trans, glm::vec3(rad));
      r_.value().Render(model_trans);
    }
  }

  float rad;
  float elasticity;
  float mass;
  std::mutex in_collision_m_;
  std::unordered_set<Sphere *> colliders_;

private:
  std::optional<Renderable> r_;
  Accessor<glm::vec3> pos_acc_;
  Accessor<glm::vec3> vel_acc_;
};
