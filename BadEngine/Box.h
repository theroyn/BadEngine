#pragma once

#include "gl_incs.h"
#include <mutex>
#include <unordered_set>
#include "utils.h"

struct Box
{
  Box(const glm::vec3 &center, const glm::vec3 &dims) : trans(1.f),
                                                        center(center),
                                                        vel(0.f),
                                                        angular_vel(0.f, 0.f, 1.f),
                                                        mass(7.f),
                                                        dims(dims),
                                                        elasticity(.9f)
  {
    trans = glm::scale(trans, dims);
    trans = glm::translate(trans, center);
    static constexpr float ANGLE = 0.f;
    float half_angle = 0.5f * (utility::PI * 0.f);
    orientation.w = cos(half_angle);
    orientation.x = sin(half_angle) * 1.f;
    orientation.y = sin(half_angle) * 0.f;
    orientation.z = sin(half_angle) * 0.f;
    orientation = glm::normalize(orientation);
  }

  glm::mat4 trans;
  glm::vec3 center;
  glm::vec3 vel;
  glm::vec3 angular_vel;
  glm::vec3 dims;
  glm::quat orientation;
  float elasticity;
  float mass;
};
