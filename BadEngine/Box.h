#pragma once

#include "gl_incs.h"
#include <mutex>
#include <unordered_set>

struct Box
{
  Box(const glm::vec3 &center, const glm::vec3 &dims) : trans(1.f),
                                                        center(center),
                                                        vel(0.f),
                                                        acc(0.f),
                                                        mass(7.f),
                                                        dims(dims),
                                                        elasticity(.9f)
  {
    trans = glm::scale(trans, dims);
    trans = glm::translate(trans, center);
  }

  glm::mat4 trans;
  glm::vec3 center;
  glm::vec3 vel;
  glm::vec3 acc;
  glm::vec3 dims;
  float elasticity;
  float mass;
};
