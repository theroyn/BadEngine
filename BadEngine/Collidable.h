#pragma once

#include "gl_incs.h"
#include "Accessor.h"
#include "State.h"

struct Collidable
{
  enum class Type
  {
    box = 0,
    sphere = 0,
    arrow = 0,
  };

  // use non-positive mass for static objects
  Collidable(Type type,
             float mass,
             glm::mat3 IBody);

  float inv_mass;
  glm::mat3 IBodyInv = glm::mat3(0.f), IInv = glm::mat3(1.f);
  float elasticity = .9f;
  glm::vec3 P;
  glm::vec3 L;

private:
  bool is_static_;
};
