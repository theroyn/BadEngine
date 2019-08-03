#pragma once

#include "Sphere.h"
#include <vector>
#include "gl_incs.h"
#include "SphereGridMap.h"
#include <tbb/blocked_range.h>

enum class sphere_coll_alg
{
  naive,
  grid
};


class CollisionSolver
{
public:
  CollisionSolver(std::vector<Sphere *> &spheres, float rad) :
                                          spheres_(spheres),
                                          center_(0.f, 0.f, 0.f),
                                          dims_(5.f),
                                          rad_(rad){}

public:
  virtual void handle_collisions() = 0;

public: // Formerly ContainingBox
  glm::vec3 dims() const { return dims_; }

protected:
  void solve_collided_spheres(Sphere *s1,
                           Sphere *s2,
                           float bumpiness = .9f);
  void handle_world_collision(Sphere *s);

private:
  void handle_world_collision_coord(Sphere *s,
                                    float glm::vec3::* coord);

protected:
  std::vector<Sphere *> &spheres_;
  const float rad_;

private:
  glm::vec3 center_; // position of the box's center.
  glm::vec3 dims_;
};

class NaiveCollisionSolver : public CollisionSolver
{
public:
  NaiveCollisionSolver(std::vector<Sphere *> &spheres, float rad) : CollisionSolver(spheres, rad)
  { }

public:
  virtual void handle_collisions();
};

class GridCollisionSolver : public CollisionSolver
{
  friend class GridRangeSolver;
public:
  GridCollisionSolver(std::vector<Sphere *> &spheres, float rad) : CollisionSolver(spheres, rad),
                                                        map_(rad_, dims())
  { }

public:
  virtual void handle_collisions();

private:
  SphereGridMap map_;
};

class SolverFactory
{
public:
  static CollisionSolver *create(sphere_coll_alg type, std::vector<Sphere *> &spheres, float radius);
};

class GridRangeSolver
{
public:
  GridRangeSolver(std::vector<Sphere *> &spheres,
                  const SphereGridMap &map,
                  GridCollisionSolver *solver);

public:
  void operator() (const tbb::blocked_range<size_t> &r) const;

private:
  std::vector<Sphere *> &spheres_;
  const SphereGridMap &map_;
  GridCollisionSolver *solver_;
};