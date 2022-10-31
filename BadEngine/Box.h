#pragma once

#include "gl_incs.h"
#include <mutex>
#include <unordered_set>
#include "utils.h"

struct Box
{
  Box(const glm::vec3 &center, const glm::vec3 &dims) : center(center),
                                                        vel(0.f),
                                                        angular_vel(0.f, 0.f, 0.f),
                                                        mass(7.f),
                                                        dims(dims),
                                                        elasticity(.9f),
                                                        color(1., .5, .71)
  {
    static constexpr float ANGLE = 0.f;
    float half_angle = 0.5f * (utility::PI * 0.f);
    orientation.w = cos(half_angle);
    orientation.x = sin(half_angle) * 1.f;
    orientation.y = sin(half_angle) * 0.f;
    orientation.z = sin(half_angle) * 0.f;
    orientation = glm::normalize(orientation);

    IBody *= (mass / 12.f);
    IBody[0][0] = dims.y * dims.y + dims.z * dims.z;
    IBody[1][1] = dims.x * dims.x + dims.z * dims.z;
    IBody[2][2] = dims.x * dims.x + dims.y * dims.y;

    IBodyInv = glm::inverse(IBody);
  }

  glm::vec3 center;
  glm::vec3 vel;
  glm::vec3 angular_vel;
  glm::vec3 dims;
  glm::quat orientation;
  glm::mat3 IBody = glm::mat3(1.f), IBodyInv = glm::mat3(1.f);
  float elasticity;
  float mass;
  glm::vec3 color;
};
