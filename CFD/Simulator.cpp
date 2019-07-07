#include "Simulator.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <gtx/norm.hpp>

static void
handle_spheres_coll(Sphere *s1, Sphere *s2, float bumpiness = .9f);

Simulator::Simulator() : h_(.05f), dampening_(.002f), spheres_n_(0)
{
}

void
handle_spheres_coll(Sphere *s1, Sphere *s2, float bumpiness)
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

void Simulator::handle_collisions()
{
  //for (auto sit = 0; sit != spheres_.size(); ++sit)
  //{
  //  Sphere *s1 = spheres_[sit];
  //  for (auto sit2 = sit + 1; sit2 != spheres_.size(); ++sit2)
  //  {
  //    handle_spheres_coll(s1, spheres_[sit2]);
  //  }

  //  world_.handle_sphere_collision(s1);
  //}
  for (auto sit = spheres_.begin(); sit != spheres_.end(); ++sit)
  {
    Sphere *s1 = *sit;
    for (auto sit2 = sit + 1; sit2 != spheres_.end(); ++sit2)
    {
      Sphere *s2 = *sit2;

      //if (glm::length2(s2->pos - s1->pos) <= s1->rad + s2->rad)
      if (glm::l2Norm(s1->pos, s2->pos) <= s1->rad + s2->rad)
        handle_spheres_coll(s1, *sit2);
    }

    world_.handle_sphere_collision(s1);
  }
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
  /*srand(time(nullptr));
  int X = 30;
  float num = ((float(rand() % X) / float(X-1)) - .5f) * 2.f;

  return num;*/
  return r3;
}
void Simulator::init()
{
  engine_.set_sphere_radius(.1f);
  engine_.init();
  

  std::vector<int> spheres_indices;
  for (int i = 0; i < spheres_n_; ++i)
    spheres_indices.push_back(engine_.add_sphere(get_rand(), get_rand(), get_rand()));
  for (int ind : spheres_indices)
  {
    //engine_.set_sphere_velocity(ind, .005f, .005f, -.001f);
    Sphere *s = engine_.get_sphere(ind);
    s->vel.x = get_rand(-.2, .2);
    s->vel.y = get_rand(-.2, .2);
    s->vel.z = get_rand(-.2, .2);
    spheres_.push_back(s);
  }

  add_global_force(glm::vec3(0.f, -.005f, 0.f)); // gravity

  engine_.set_world_dims(world_.dims());
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


void
ContainingBox::handle_sphere_coord_collision(Sphere *s,
                                             float glm::vec3::*coord)
{
  if (s->pos.*coord - s->rad <= pos_.*coord - dims_.*coord / 2 && s->vel.*coord < 0)
    s->vel.*coord *= -s->bounciness;

  if (s->pos.*coord + s->rad >= pos_.*coord + dims_.*coord / 2 && s->vel.*coord > 0)
    s->vel.*coord *= -s->bounciness;
}

void ContainingBox::handle_sphere_collision(Sphere *s)
{
  handle_sphere_coord_collision(s, &glm::vec3::x);
  handle_sphere_coord_collision(s, &glm::vec3::y);
  handle_sphere_coord_collision(s, &glm::vec3::z);
}