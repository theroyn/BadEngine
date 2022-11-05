#include "CollisionSolver.h"

#include <glm/gtx/norm.hpp>
#include <tbb/parallel_for.h>
#include <thread>
#include <mutex>

/*******************************************************************************
 * class CollisionSolver Implementation
 */
void CollisionSolver::solve_collided_spheres(Sphere *s1,
                                             Sphere *s2,
                                             float elasticity)
{
  bool should_update = false;

  {
    std::scoped_lock lock(s1->in_collision_m_, s2->in_collision_m_);

    s1->colliders_.insert(s2);
    s2->colliders_.insert(s1);
    should_update = true;
  }

  if (should_update)
  {
    glm::vec3 n = glm::normalize(s1->get_pos() - s2->get_pos());
    glm::vec3 vrel = s1->get_vel() - s2->get_vel();
    float verl_scl = glm::dot(vrel, n);

    if (verl_scl < 0)
    {
      float imp_nom = -1 * (1 + elasticity) * verl_scl;
      float imp_denom = (1 / s1->mass) + (1 / s2->mass);
      float imp = imp_nom / imp_denom;

      s1->set_vel(s1->get_vel() + (imp / s1->mass) * n);
      s2->set_vel(s2->get_vel() - (imp / s2->mass) * n);
    }
  }
}

void CollisionSolver::handle_world_collision2_coord(Sphere *s,
                                                    float glm::vec3::*coord)
{
  if (s->get_pos().*coord - s->rad <= center_.*coord - dims_.*coord / 2 && s->get_vel().*coord < 0)
  {
    glm::vec3 v = s->get_vel();
    v.*coord *= -s->elasticity;
    s->set_vel(v);
    //s->pos.*coord = center_.*coord - dims_.*coord / 2 + s->rad;
  }

  if (s->get_pos().*coord + s->rad >= center_.*coord + dims_.*coord / 2 && s->get_vel().*coord > 0)
  {
    glm::vec3 v = s->get_vel();
    v.*coord *= -s->elasticity;
    s->set_vel(v);
    //s->pos.*coord = center_.*coord + dims_.*coord / 2 - s->rad;
  }
}

void CollisionSolver::handle_world_collision2(Sphere *s)
{
  handle_world_collision2_coord(s, &glm::vec3::x);
  handle_world_collision2_coord(s, &glm::vec3::y);
  handle_world_collision2_coord(s, &glm::vec3::z);
}

void CollisionSolver::handle_world_collision(Sphere *s)
{
  // todo: find a way not to lock here
  std::scoped_lock lock(s->in_collision_m_);

  glm::vec3 n(0.f);
  for (int i = 0; i < 3; ++i)
  {
    if (s->get_pos()[i] + s->rad >= center_[i] + dims_[i] * .5f)
    {
      n[i] = -1.f;
    }
    else if (s->get_pos()[i] - s->rad <= center_[i] - dims_[i] * .5f)
    {
      n[i] = +1.f;
    }
  }

  n = glm::normalize(n);

  glm::vec3 vrel = s->get_vel();
  float verl_scl = glm::dot(vrel, n);

  if (verl_scl < 0)
  {
    float imp_nom = -1 * (1 + s->elasticity) * verl_scl;
    float imp_denom = (1 / s->mass) + 0.f /** wall mass treated as infinite */;
    float imp = imp_nom / imp_denom;

    s->set_vel(s->get_vel() + (imp / s->mass) * n);
  }
}

/*******************************************************************************
 * class NaiveCollisionSolver Implementation
 */
void NaiveCollisionSolver::handle_collisions(const std::vector<Sphere *> &spheres)
{
  for (auto sit1 = spheres.begin(); sit1 != spheres.end(); ++sit1)
  {
    Sphere *s1 = *sit1;
    for (auto sit2 = sit1 + 1; sit2 != spheres.end(); ++sit2)
    {
      Sphere *s2 = *sit2;

      if (glm::l2Norm(s1->get_pos(), s2->get_pos()) <= s1->rad + s2->rad)
        solve_collided_spheres(s1, *sit2);
    }

    handle_world_collision(s1);
  }
}
GridRangeSolver::GridRangeSolver(const std::vector<Sphere *> &spheres,
                                 const SphereGridMap &map,
                                 GridCollisionSolver *solver) : spheres_(spheres),
                                                                map_(map),
                                                                solver_(solver) {}

void GridRangeSolver::operator()(const tbb::blocked_range<size_t> &r) const
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

      if (glm::l2Norm(s1->get_pos(), s2->get_pos()) <= s1->rad + s2->rad)
        solver_->solve_collided_spheres(s1, s2);
    }

    // DUDU investigate why this throws
    //assert(s1_inside);

    solver_->handle_world_collision(s1);
  }
}
/*******************************************************************************
 * class GridCollisionSolver Implementation
 */
void GridCollisionSolver::handle_collisions(const std::vector<Sphere *> &spheres)
{
  map_.update_map(spheres);

  tbb::parallel_for(tbb::blocked_range<size_t>(0, spheres.size()), GridRangeSolver(spheres, map_, this));
}

/*******************************************************************************
 * class SolverFactory Implementation
 */
CollisionSolver *SolverFactory::create(sphere_coll_alg type,
                                       float radius)
{
  CollisionSolver *sol = nullptr;

  switch (type)
  {
  case sphere_coll_alg::naive:
    sol = new NaiveCollisionSolver();
    break;
  case sphere_coll_alg::grid:
    sol = new GridCollisionSolver(radius);
    break;
  default:
    assert(0);
  }

  return sol;
}