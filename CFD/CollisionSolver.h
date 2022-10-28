#pragma once

#include "Sphere.h"
#include <vector>
#include <shared_mutex>
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
  CollisionSolver() : center_(0.f, 0.f, 0.f),
                      dims_(5.f) {}

public:
  virtual void handle_collisions(const std::vector<Sphere *> &spheres) = 0;

public: // Formerly ContainingBox
  glm::vec3 dims() const { return dims_; }

protected:
  void solve_collided_spheres(Sphere *s1,
                              Sphere *s2,
                              // 0 = inelastic, 1 = perfectly elastic
                              float elasticity = .9f);
  void handle_world_collision(Sphere *s);
  void handle_world_collision2(Sphere *s);

private:
  void handle_world_collision2_coord(Sphere *s,
                                     float glm::vec3::*coord);

private:
  glm::vec3 center_; // position of the box's center.
  glm::vec3 dims_;
};

class NaiveCollisionSolver : public CollisionSolver
{
public:
  NaiveCollisionSolver() : CollisionSolver()
  {
  }

public:
  virtual void handle_collisions(const std::vector<Sphere *> &spheres) override;
};

struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const
  {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

class GridCollisionSolver : public CollisionSolver
{
  friend class GridRangeSolver;

public:
  // todo: execution order is correct atm(CollisionSolver before map_), but do this better
  GridCollisionSolver(float rad) : CollisionSolver(),
                                   map_(rad, dims())
  {
  }

public:
  virtual void handle_collisions(const std::vector<Sphere *> &spheres) override;

private:
  SphereGridMap map_;
  std::shared_mutex colliders_mutex_;
};

class SolverFactory
{
public:
  static CollisionSolver *create(sphere_coll_alg type, float radius = 0.f);
};

class GridRangeSolver
{
public:
  GridRangeSolver(const std::vector<Sphere *> &spheres,
                  const SphereGridMap &map,
                  GridCollisionSolver *solver);

public:
  void operator()(const tbb::blocked_range<size_t> &r) const;

private:
  const std::vector<Sphere *> &spheres_;
  const SphereGridMap &map_;
  GridCollisionSolver *solver_;
};