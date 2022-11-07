#pragma once

#include "gl_incs.h"

struct State
{
  State(const glm::vec3 &p,
        const glm::vec3 &v) : p(p),
                              v(v),
                              orientation(glm::identity<glm::quat>()) {}

  glm::vec3 p;
  glm::vec3 v;
  glm::quat orientation;
};