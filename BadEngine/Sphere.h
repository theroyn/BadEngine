#pragma once

#include "gl_incs.h"
#include "Renderable.h"
#include "Accessor.h"
#include "Shape.h"
#include "State.h"

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
         Accessor<State> state_acc) : Shape(state_acc,
                                            glm::vec3(rad)),
                                      mass(7.f),
                                      rad(rad),
                                      elasticity(.9f)
  {
    set_pos(glm::vec3(x, y, z));
    set_vel(glm::vec3(0.f));
  }

private:
  virtual Collidable create_collidable(float mass) const override
  {
    glm::mat3 IBody = glm::identity<glm::mat3>() * (2.f / 5.f) * mass * rad * rad; // (2/5)*m*r^2

    return Collidable(Collidable::Type::sphere, mass, IBody);
  }

public:
  float rad;
  float elasticity;
  float mass;
  std::mutex in_collision_m_;
  std::unordered_set<Sphere *> colliders_;
};
