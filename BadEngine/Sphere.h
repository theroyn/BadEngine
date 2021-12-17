#pragma once

#include "gl_incs.h"

struct Sphere
{
  Sphere(float x, float y, float z, float rad) : pos(x, y, z),
                                                 vel(0.f), 
                                                 acc(0.f), 
                                                 mass(7.f), 
                                                 rad(rad),
                                                 bounciness(.2f) {}

  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec3 acc;
  float rad;
  float bounciness;
  float mass;
};
