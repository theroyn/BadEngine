#pragma once

#include "gl_incs.h"
#include <mutex>
#include <unordered_set>

struct Sphere
{
  Sphere(float x, float y, float z, float rad) : pos(x, y, z),
                                                 vel(0.f), 
                                                 acc(0.f), 
                                                 mass(7.f), 
                                                 rad(rad),
                                                 elasticity(.9f) {}

  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec3 acc;
  float rad;
  float elasticity;
  float mass;
  std::mutex in_collision_m_;
  std::unordered_set<Sphere *> colliders_;
};
