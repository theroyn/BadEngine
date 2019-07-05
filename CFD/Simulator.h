#pragma once

#include "gl_incs.h"
#include <vector>
#include "BadEngine.h"

class ContainingBox
{
public:
  ContainingBox() : pos_(0.f, 0.f, 0.f), dims_(3.f) {}
  ContainingBox(const SimpleBox &box) : pos_(box.pos), dims_(3.f) {}

public:
  void handle_sphere_collision(Sphere *s);
  void handle_sphere_coord_collision(const glm::vec3 &pos,
                                     glm::vec3 &vel,
                                     float rad,
                                     float glm::vec3::* coord);
  glm::vec3 dims() const { return dims_; }


private:
  glm::vec3 pos_; // position of the box's center.
  glm::vec3 dims_;
};

class Simulator
{
public:
  Simulator();
  ~Simulator();

public:
  void add_global_force(glm::vec3 f);

public:
  void run();
  void init();

private:
  void integrate();
  void handle_collisions();

private:
  float h_, dampening_;
  BadEngine engine_;
  std::vector<glm::vec3> g_forces_;
  std::vector<Sphere *> spheres_;
  ContainingBox world_;
};