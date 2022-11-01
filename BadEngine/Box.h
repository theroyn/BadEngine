#pragma once

#include "gl_incs.h"
#include <mutex>
#include <unordered_set>
#include "utils.h"

struct Box
{
  Box(const glm::vec3 &center, const glm::vec3 &dims, bool is_static = false) : center(center),
                                                                                vel(0.f),
                                                                                P(0.f),
                                                                                L(0.f),
                                                                                angular_vel(0.f, 0.f, 0.f),
                                                                                inv_mass(is_static ? 0.f : (1.f / 7.f)),
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

    if (!is_static)
    {
      glm::mat3 IBody = glm::mat3(1.f) * ((1.f / inv_mass) / 12.f);
      IBody[0][0] = dims.y * dims.y + dims.z * dims.z;
      IBody[1][1] = dims.x * dims.x + dims.z * dims.z;
      IBody[2][2] = dims.x * dims.x + dims.y * dims.y;

      IBodyInv = glm::inverse(IBody);
    }
  }

  void set_initial_vel(const glm::vec3 &v)
  {
    vel = v;
    if (inv_mass > .001)
    {
      P = vel / inv_mass;
    }
  }

  glm::vec3 center;
  glm::vec3 vel;
  glm::vec3 P;
  glm::vec3 L;
  glm::vec3 angular_vel;
  glm::vec3 dims;
  glm::quat orientation;
  glm::mat3 IBodyInv = glm::mat3(0.f), IInv = glm::mat3(1.f);
  float elasticity;
  float inv_mass;
  glm::vec3 color;
};
