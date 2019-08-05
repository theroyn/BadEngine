#pragma once

#include "gl_incs.h"
#include <vector>
#include "BadEngine.h"
#include "Sphere.h"
#include "CollisionSolver.h"


class Simulator
{
public:
  Simulator(unsigned int spheres_n);
  ~Simulator();

public:
  void add_global_force(glm::vec3 f);

public:
  void run();
  void init();

private:
  void integrate();
  void handle_collisions();
  void handle_sphere_collisions_naive_alg();

private:
  float h_, dampening_;
  const float base_h_;
  unsigned int spheres_n_;
  float sphere_rad_;
  float last_time_;
  BadEngine engine_;
  std::vector<glm::vec3> g_forces_;
  std::vector<Sphere *> spheres_;
  CollisionSolver *col_solver_;
  const sphere_coll_alg sphere_coll_alg_;
};
