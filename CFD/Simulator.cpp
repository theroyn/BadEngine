#include "Simulator.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <glm/gtx/norm.hpp>
#include <functional>


Simulator::Simulator(unsigned int spheres_n) : sphere_coll_alg_(sphere_coll_alg::grid),
                                               h_(.03f),
                                               base_h_(h_),
                                               dampening_(.009f),
                                               spheres_n_(spheres_n),
                                               sphere_rad_(.1f),
                                               engine_(std::bind(&Simulator::key_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))
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

void Simulator::add_global_force(const std::string &name, glm::vec3 f)
{
  // override existing force if there is one
  g_forces_[name] = f;
}
void Simulator::remove_global_force(const std::string &name)
{
  auto it = g_forces_.find(name);
  if (it == g_forces_.end())
  {
    std::cerr << "No force named " << name << "\n";
  }
  else
  {
    g_forces_.erase(it);
  }
}

void Simulator::integrate()
{
  static bool init = true;

  float curr_time = ( float )glfwGetTime();

  if (init)
  {
    init = false;
    last_time_ = curr_time;
  }

  float delta = curr_time - last_time_;
  last_time_ = curr_time;

  h_ = base_h_ * 133.33f * delta;

  for (auto sphere : spheres_)
  {
    sphere->acc = glm::vec3(0.f);

    for (auto f : g_forces_)
    {
      sphere->acc += f.second / sphere->mass;
    }

    // internal forces calculations
    glm::vec3 dampening_force = -dampening_ * sphere->vel;
    sphere->acc += dampening_force / sphere->mass;
    static Sphere *lowSphere = nullptr;
    static size_t cnt = 0;

    if (!lowSphere && sphere->pos.y < -2.5f)
    {
      lowSphere = sphere;
    }
    if (lowSphere == sphere && cnt++ % 100 == 0)
    {
      std::cout <<"h_:"<<h_<< ", prev pos (" << lowSphere->pos.x << "," << lowSphere->pos.y << "," << lowSphere->pos.z << ")\n";
    }
    // pos integration before velocity, since integration is relative to values from previous step.
    sphere->pos += h_ * sphere->vel;
    if (lowSphere == sphere && cnt % 100 == 0)
    {
      std::cout << "curr pos (" << lowSphere->pos.x << "," << lowSphere->pos.y << "," << lowSphere->pos.z << ")\n\n";
    }
    sphere->vel += h_ * sphere->acc;

    sphere->colliders_.clear();
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
  
  std::vector<size_t> spheres_indices;
  glm::vec3 dims = col_solver_->dims();
  float w = dims.x / 2.f;
  float h = dims.y / 2.f;
  float d = dims.z / 2.f;
  const bool small_start = false;

  for (unsigned int i = 0; i < spheres_n_; ++i)
  {
    if (small_start)
      spheres_indices.push_back(engine_.add_sphere(get_rand(), get_rand(), get_rand()));
    else
      spheres_indices.push_back(engine_.add_sphere(get_rand(-w, w), get_rand(-h, h), get_rand(-d, d)));
  }

  for (size_t ind : spheres_indices)
  {
    Sphere *s = engine_.get_sphere(ind);
    s->vel.x = get_rand(-.2f, .2f);
    s->vel.y = get_rand(-.2f, .2f);
    s->vel.z = get_rand(-.2f, .2f);
    spheres_.push_back(s);
  }

  add_global_force("gravity", glm::vec3(0.f, -.9f, 0.f));

  engine_.set_world_dims(col_solver_->dims());
}

namespace cr = std::chrono;
static void print_fps()
{
  static size_t frame_cnt = 0;
  frame_cnt++;
  static auto last_print = cr::system_clock::now().time_since_epoch();
  static auto previous = cr::system_clock::now().time_since_epoch();
  auto current = cr::system_clock::now().time_since_epoch();
  cr::microseconds diff_from_last_print = cr::duration_cast<cr::microseconds>(current - last_print);
  if (diff_from_last_print >= cr::seconds(1))
  {
    last_print = current;

    cr::microseconds diff_from_last_frame = cr::duration_cast<cr::microseconds>(current - previous);
    std::cout << "frame num:" << frame_cnt << ", Time to process last frame (milliseconds): " << diff_from_last_frame.count() / 1000.0
              << " FPS: " << 1.0 / ((double)diff_from_last_frame.count() / 1000'000.0) << "\n";
  }
  previous = current;
}

void Simulator::run()
{
  while (!engine_.loop_done())
  {
    engine_.draw();

    handle_collisions();

    integrate();
    print_fps();
  }
}

void Simulator::key_callback(int key, int scancode, int action, int mods)
{
  switch (key)
  {
  case GLFW_KEY_P:
  {
    if (action == GLFW_PRESS)
    {
      std::cout << "P press!!!\n";

      glm::vec3 force(get_rand(-1.f, 1.f), .5f, get_rand(-1.f, 1.f));
      std::cout << "adding:(" << force.x << "," << force.y << "," << force.z << "\n";
      add_global_force("P-force", force);
    }
    else if (action == GLFW_RELEASE)
    {
      std::cout << "P release!!!\n";
      remove_global_force("P-force");
    }
    else if (action == GLFW_REPEAT)
    {
      // ignored
    }
    else
    {
      std::cout << "P unknown action:" << action << "\n";
    }
  }
  break;
  default:
  {
    // ignore
  }
  }
}
