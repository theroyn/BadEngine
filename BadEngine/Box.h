#pragma once

#include "gl_incs.h"
#include "Shape.h"
#include "State.h"

#include <mutex>
#include <unordered_set>
#include "utils.h"

class Box : public Shape
{
public:
  Box(Accessor<State> state_acc,
      const glm::vec3 &center,
      const glm::vec3 &dims) : Shape(state_acc),
                               dims(dims),
                               color(1., .5, .71)
  {
    set_pos(center);
    set_vel(glm::vec3(0.f));

    float half_angle = 0.5f * (utility::PI * 0.f);

    set_orientation(glm::normalize((glm::quat(cos(half_angle),
                                              sin(half_angle) * 1.f,
                                              sin(half_angle) * 0.f,
                                              sin(half_angle) * 0.f))));
  }

  virtual Collidable create_collidable(float mass) const override
  {
    glm::mat3 IBody = glm::identity<glm::mat3>() * (mass / 12.f);
    IBody[0][0] = dims.y * dims.y + dims.z * dims.z;
    IBody[1][1] = dims.x * dims.x + dims.z * dims.z;
    IBody[2][2] = dims.x * dims.x + dims.y * dims.y;

    return Collidable(Collidable::Type::box, mass, IBody);
  }

  void set_initial_vel(const glm::vec3 &v)
  {
    set_vel(v);
    if (has_collidable() && get_collidable().inv_mass > .001)
    {
      get_collidable().P = get_vel() / get_collidable().inv_mass;
    }
  }

  glm::vec3 dims;
  glm::vec3 color;
};
