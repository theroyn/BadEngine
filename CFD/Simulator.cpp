#include "Simulator.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

void Simulator::handle_collisions()
{
  for (auto s : spheres_)
  {
    world_.handle_sphere_collision(s);
  }
}

Simulator::Simulator() : h_(.05f), dampening_(.002f)
{
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
  for (int i = 0; i < 500; ++i)
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
ContainingBox::handle_sphere_coord_collision(const glm::vec3 &pos,
                                             glm::vec3 &vel,
                                             float rad,
                                             float glm::vec3::* coord)
{
  if (pos.*coord - rad <= pos_.*coord - dims_.*coord / 2 && vel.*coord < 0)
    vel.*coord *= -.8;

  if (pos.*coord + rad >= pos_.*coord + dims_.*coord / 2 && vel.*coord > 0)
    vel.*coord *= -.8;
}

void ContainingBox::handle_sphere_collision(Sphere *s)
{
  const glm::vec3 &p = s->pos;

  handle_sphere_coord_collision(s->pos, s->vel, s->rad, &glm::vec3::x);
  handle_sphere_coord_collision(s->pos, s->vel, s->rad, &glm::vec3::y);
  handle_sphere_coord_collision(s->pos, s->vel, s->rad, &glm::vec3::z);
}