#include "Simulator.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <gtx/norm.hpp>


Simulator::Simulator(unsigned int spheres_n) : sphere_coll_alg_(sphere_coll_alg::grid),
                         h_(.08f),
                         dampening_(.001f),
                         spheres_n_(spheres_n),
                         sphere_rad_(.1f)
{
  col_solver_ = SolverFactory::create(sphere_coll_alg_, spheres_, sphere_rad_);
}

void Simulator::handle_collisions()
{
  col_solver_->handle_collisions();
}

Simulator::~Simulator()
{
}

void Simulator::add_global_force(glm::vec3 f)
{
  g_forces_.push_back(f);
}

void Simulator::integrate()
{
  for (auto sphere : spheres_)
  {
    sphere->acc = std::move(glm::vec3(0.f));

    for (auto f : g_forces_)
    {
      sphere->acc += f / sphere->mass;
    }

    // internal forces calculations
    glm::vec3 dampening_force = -dampening_ * sphere->vel;
    sphere->acc += dampening_force / sphere->mass;

    glm::vec3 vel = sphere->vel + h_ * sphere->acc;
    sphere->pos += h_ * sphere->vel;
    sphere->vel = vel;
  }
}
static float get_rand(float low=-.5f, float high = .5f)
{
  float r3 = low + static_cast < float > (rand()) / (static_cast < float > (RAND_MAX / (high - low)));

  return r3;
}
void Simulator::init()
{
  engine_.set_sphere_radius(sphere_rad_);
  engine_.init();
  
  std::vector<int> spheres_indices;
  glm::vec3 dims = col_solver_->dims();
  float w = dims.x / 2.;
  float h = dims.y / 2.;
  float d = dims.z / 2.;
  const bool small_start = false;

  for (int i = 0; i < spheres_n_; ++i)
  {
    if (small_start)
      spheres_indices.push_back(engine_.add_sphere(get_rand(), get_rand(), get_rand()));
    else
      spheres_indices.push_back(engine_.add_sphere(get_rand(-w, w), get_rand(-h, h), get_rand(-d, d)));
  }

  for (int ind : spheres_indices)
  {
    Sphere *s = engine_.get_sphere(ind);
    s->vel.x = get_rand(-.2, .2);
    s->vel.y = get_rand(-.2, .2);
    s->vel.z = get_rand(-.2, .2);
    spheres_.push_back(s);
  }

  add_global_force(glm::vec3(0.f, -.1f, 0.f)); // gravity

  engine_.set_world_dims(col_solver_->dims());
}


void Simulator::run()
{
  while (!engine_.loop_done())
  {
    engine_.draw();

    handle_collisions();

    integrate();
  }
}
