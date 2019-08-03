#include "CollisionSolver.h"

#include <gtx/norm.hpp>
#include <tbb/parallel_for.h>

/*******************************************************************************
 * class CollisionSolver Implementation
 */
void CollisionSolver::solve_collided_spheres(Sphere *s1,
                                          Sphere *s2,
                                          float bumpiness)
{
  glm::vec3 n = glm::normalize(s1->pos - s2->pos);
  glm::vec3 vrel = s1->vel - s2->vel;
  float verl_scl = glm::dot(vrel, n);

  if (verl_scl < 0)
  {
    float imp_nom = -1 * (1 + bumpiness) * verl_scl;
    float imp_denom = (1 / s1->mass) + (1 / s2->mass);
    float imp = imp_nom / imp_denom;

    s1->vel += (imp / s1->mass) * n;
    s2->vel -= (imp / s2->mass) * n;
  }
}

void
CollisionSolver::handle_world_collision_coord(Sphere *s,
                                             float glm::vec3::*coord)
{
  if (s->pos.*coord - s->rad <= center_.*coord - dims_.*coord / 2 && s->vel.*coord < 0)
    s->vel.*coord *= -s->bounciness;

  if (s->pos.*coord + s->rad >= center_.*coord + dims_.*coord / 2 && s->vel.*coord > 0)
    s->vel.*coord *= -s->bounciness;
}

void CollisionSolver::handle_world_collision(Sphere *s)
{
  handle_world_collision_coord(s, &glm::vec3::x);
  handle_world_collision_coord(s, &glm::vec3::y);
  handle_world_collision_coord(s, &glm::vec3::z);
}

/*******************************************************************************
 * class NaiveCollisionSolver Implementation
 */
void NaiveCollisionSolver::handle_collisions()
{
  for (auto sit = spheres_.begin(); sit != spheres_.end(); ++sit)
  {
    Sphere *s1 = *sit;
    for (auto sit2 = sit + 1; sit2 != spheres_.end(); ++sit2)
    {
      Sphere *s2 = *sit2;

      if (glm::l2Norm(s1->pos, s2->pos) <= s1->rad + s2->rad)
        solve_collided_spheres(s1, *sit2);
    }

    handle_world_collision(s1);
  }
}
GridRangeSolver::GridRangeSolver(std::vector<Sphere *> &spheres,
                  const SphereGridMap &map,
                  GridCollisionSolver *solver) :
                                              spheres_(spheres),
                                              map_(map),
                                              solver_(solver) {}

void GridRangeSolver::operator() (const tbb::blocked_range<size_t>& r) const
{
  for (size_t i = r.begin(); i != r.end(); ++i)
  {
    Sphere *s1 = spheres_[i];

    std::list<Sphere *> nbs;

    map_.get_neighbours(s1, nbs);

    bool s1_inside = false;

    for (auto s2 : nbs)
    {
      if (s1 == s2)
      {
        s1_inside = true;
        continue;
      }

      if (glm::l2Norm(s1->pos, s2->pos) <= s1->rad + s2->rad)
        solver_->solve_collided_spheres(s1, s2);
    }
    assert(s1_inside);

    solver_->handle_world_collision(s1);
  }
}
/*******************************************************************************
 * class GridCollisionSolver Implementation
 */
void GridCollisionSolver::handle_collisions()
{
  map_.update_map(spheres_);

  tbb::parallel_for(tbb::blocked_range<size_t>(0, spheres_.size()), GridRangeSolver(spheres_, map_, this));

}

/*******************************************************************************
 * class SolverFactory Implementation
 */
CollisionSolver *SolverFactory::create(sphere_coll_alg type,

                                       std::vector<Sphere *> &spheres, float radius)
{
  CollisionSolver *sol = nullptr;

  switch (type)
  {
  case sphere_coll_alg::naive:
    sol = new NaiveCollisionSolver(spheres, radius);
    break;
  case sphere_coll_alg::grid:
    sol = new GridCollisionSolver(spheres, radius);
    break;
  default:
    assert(0);
  }

  return sol;
}