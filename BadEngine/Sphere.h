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
         Accessor<State> state_acc) : Shape(state_acc),
                                      mass(7.f),
                                      rad(rad),
                                      elasticity(.9f)
  {
    set_pos(glm::vec3(x, y, z));
    set_vel(glm::vec3(0.f));
  }

  float rad;
  float elasticity;
  float mass;
  std::mutex in_collision_m_;
  std::unordered_set<Sphere *> colliders_;
};
