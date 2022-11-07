#pragma once

#include "gl_incs.h"

struct State
{
  State(const glm::vec3 &p,
        const glm::vec3 &v) : p(p),
                              orientation(glm::identity<glm::quat>()),
                              v(v),
                              angular_vel(0.f) {}

  glm::vec3 p;
  glm::quat orientation;
  glm::vec3 v;
  glm::vec3 angular_vel;
};