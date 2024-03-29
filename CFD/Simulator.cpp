#include "Simulator.h"

#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <glm/gtx/norm.hpp>
#include <functional>
#include <utils.h>

static const glm::vec3 GRAVITY(0.f, -.9f, 0.f);

Simulator::Simulator(unsigned int spheres_n,
                     unsigned int boxes_n) : sphere_coll_alg_(sphere_coll_alg::grid),
                                             base_h_(.03f),
                                             damping_(.09f),
                                             spheres_n_(spheres_n),
                                             boxes_n_(boxes_n),
                                             sphere_rad_(.1f),
                                             engine_(std::bind(&Simulator::key_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)),
                                             impulse_solver_(this)
{
  col_solver_ = SolverFactory::create(sphere_coll_alg_, sphere_rad_);

  // Create the world settings
  reactphysics3d::PhysicsWorld::WorldSettings settings;
  settings.gravity = reactphysics3d::Vector3(GRAVITY.x, GRAVITY.y, GRAVITY.z);

  // Create the physics world with your settings
  world_ = physics_common_.createPhysicsWorld(settings);
}

void Simulator::handle_collisions()
{
  // Sphere collisions
  //col_solver_->handle_collisions(spheres_);

  // Shape collisions
  int solver_iteration_counter = 0;

  do
  {
    if (solver_iteration_counter == 30)
    {
      break;
    }

    impulse_solver_.clear();

    world_->testCollision(impulse_solver_);

    if (impulse_solver_.has_contacts())
    {
      impulse_solver_.solve();
      solver_iteration_counter++;
    }
  } while (impulse_solver_.had_collisions());

  if (solver_iteration_counter > 0)
  {
    //std::cout << "Solved after " << solver_iteration_counter << " iterations\n";
  }
}

Simulator::~Simulator()
{
  for (reactphysics3d::CollisionBody *body : bodies_)
  {
    world_->destroyCollisionBody(body);
  }
  physics_common_.destroyPhysicsWorld(world_);
}

void Simulator::add_global_force(const std::string &name, glm::vec3 f)
{
  // override existing force if there is one
  g_forces_[name] = f;
}
void Simulator::add_global_torque(const std::string &name, glm::vec3 f)
{
  // override existing force if there is one
  g_torques_[name] = f;
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

void Simulator::remove_global_torque(const std::string &name)
{
  auto it = g_torques_.find(name);
  if (it == g_torques_.end())
  {
    std::cerr << "No torque named " << name << "\n";
  }
  else
  {
    g_torques_.erase(it);
  }
}

static void projectile(Arrow *arrow)
{
  static std::once_flag of;
  static double kin_start_time_ = glfwGetTime();
  std::call_once(of, [&]()
                 {
                   float theta_start_deg = 45.f;
                   float theta_start = theta_start_deg * static_cast<float>(utility::PI) / 180.f;
                   float s_start_deg = 180.f; // angle between x-z axii
                   float s_start = s_start_deg * static_cast<float>(utility::PI) / 180.f;
                   float v0 = 3.f;
                   float v0xz = v0 * cos(theta_start);
                   float v0y = v0 * sin(theta_start);
                   float v0x = v0xz * cos(s_start);
                   float v0z = v0xz * sin(s_start);

                   arrow->vel_start = glm::vec3(v0x, v0y, v0z);
                   arrow->theta = theta_start;
                   bool is_eq = (abs(glm::l2Norm(arrow->vel_start) - v0) < .0001f);

                   // orientation
                   glm::vec3 v = glm::normalize(arrow->vel_start);
                   glm::vec3 unit = glm::normalize(glm::vec3(0.f, 1.f, 0.f) - glm::vec3(0.f, 0.f, 0.f));
                   float theta = glm::dot(unit, v);
                   glm::vec3 axis = glm::normalize(glm::cross(unit, v));

                   float half_angle = 0.5f * theta;

                   arrow->set_orientation(glm::normalize((glm::quat(cos(half_angle),
                                                                    sin(half_angle) * 1.f,
                                                                    sin(half_angle) * 0.f,
                                                                    sin(half_angle) * 0.f))));
                 });

  double t = glfwGetTime();
  float delta = static_cast<float>(t - kin_start_time_) * 3.f;

  glm::vec3 g = GRAVITY;
  glm::vec3 curr_v = arrow->vel_start + delta * g;
  //  accurate because acceleration is constant
  arrow->set_pos(arrow->pos_start + delta * arrow->vel_start + 0.5f * delta * delta * g);
  arrow->orient(curr_v);
  float tmax = arrow->vel_start.y / (-g.y);
  bool is_tmax = (abs(delta - tmax) < .01f);
  if (is_tmax)
  {
    std::cout << "\n************************************\n";
  }
}

static void circle(Arrow *arrow)
{
  static std::once_flag of;
  static double last_time = glfwGetTime();
  // inputs
  static float rad = 1.f;
  static glm::vec3 p0(1.f, 0.f, 0.f);
  static glm::vec3 v0(0.f, 1.f, 0.f);
  std::call_once(of, [&]()
                 {
                   arrow->vel_start = v0;
                   arrow->set_vel(arrow->vel_start);
                   arrow->pos_start = p0;
                   arrow->set_pos(arrow->pos_start);

                   // orientation
                   arrow->orient(p0);
                 });
  double t = glfwGetTime();
  float delta = static_cast<float>(t - last_time) * 3.f;
  last_time = t;

  float v_length = glm::l2Norm(v0);
  float ar = v_length * v_length / rad;

  glm::vec3 a = glm::normalize(-arrow->get_pos()) * ar;

  arrow->set_vel(arrow->get_vel() + delta * a);
  arrow->set_pos(arrow->get_pos() + delta * arrow->get_vel());
  arrow->orient(a);
}

static void circle2(Arrow *arrow)
{
  static std::once_flag of;
  static double last_time = glfwGetTime();
  // inputs
  static float rad = 1.f;
  static glm::vec3 p0(1.f, 0.f, 0.f);
  static glm::vec3 v0(0.f, 1.f, 0.f);
  std::call_once(of, [&]()
                 {
                   arrow->vel_start = v0;
                   arrow->set_vel(arrow->vel_start);
                   arrow->pos_start = p0;
                   arrow->set_pos(arrow->pos_start);

                   // orientation
                   arrow->orient(p0);
                 });
  double t = glfwGetTime();
  float delta = static_cast<float>(t - last_time);

  float v_length = glm::l2Norm(v0);
  float ar = v_length * v_length / rad;

  glm::vec3 a = glm::normalize(-arrow->get_pos()) * ar;

  arrow->set_vel(arrow->get_vel() + delta * a);
  arrow->set_pos(rad * glm::vec3(cos(delta), sin(delta), 0.f));
  delta * arrow->get_vel();
  arrow->orient(glm::normalize(-arrow->get_pos()));
}

void Simulator::kinematics()
{
  static Arrow *arrow_proj = engine_.get_arrow(engine_.add_arrow(glm::vec3(0.f, 0.f, 0.f), glm::vec3(.5f, 1.f, .5f), true));
  static Arrow *arrow_circle = engine_.get_arrow(engine_.add_arrow(glm::vec3(0.f, 0.f, 0.f), glm::vec3(.5f, 1.f, .5f), true));

  projectile(arrow_proj);
  circle2(arrow_circle);

  arrow_proj->update_model_if_renderable(arrow_proj->get_dims());
  arrow_circle->update_model_if_renderable(arrow_circle->get_dims());
}

void Simulator::integrate()
{
  static bool init = true;

  double curr_time = glfwGetTime();

  if (init)
  {
    init = false;
    last_time_ = curr_time;
  }

  float delta = (float)(curr_time - last_time_);
  last_time_ = curr_time;

  float h = base_h_ * 133.33f * delta;
  //integrate_spheres(h);
  integrate_shapes(h);
}

static float get_rand(float low = -.5f, float high = .5f)
{
  float r3 = low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (high - low)));

  return r3;
}

void Simulator::integrate_spheres(float h)
{
  for (Sphere *sphere : spheres_)
  {
    glm::vec3 acc(0.f);

    for (const auto &f : g_forces_)
    {
      acc += f.second / sphere->mass;
    }

    // internal forces calculations
    glm::vec3 damping_force = -damping_ * sphere->get_vel();
    acc += damping_force / sphere->mass;
    static Sphere *lowSphere = nullptr;
    static size_t cnt = 0;

    if (!lowSphere && sphere->get_pos().y < -2.5f)
    {
      lowSphere = sphere;
    }
    if (lowSphere == sphere && cnt++ % 100 == 0)
    {
      std::cout << "h:" << h << ", prev pos (" << lowSphere->get_pos().x << "," << lowSphere->get_pos().y << "," << lowSphere->get_pos().z << ")\n";
    }
    // semi-implicit euler integration
    sphere->set_vel(sphere->get_vel() + h * acc);
    sphere->set_pos(sphere->get_pos() + h * sphere->get_vel());
    if (lowSphere == sphere && cnt % 100 == 0)
    {
      std::cout << "curr pos (" << lowSphere->get_pos().x << "," << lowSphere->get_pos().y << "," << lowSphere->get_pos().z << ")\n\n";
    }

    sphere->colliders_.clear();

    sphere->update_model_if_renderable(glm::vec3(sphere->rad)); // DUDU identity orientation
  }
}

void Simulator::integrate_shapes(float h)
{
  // DUDU refactor this ugliness
  std::vector<Shape *> shapes;
  std::copy(boxes_.begin(), boxes_.end(), std::back_inserter(shapes));
  std::copy(spheres_.begin(), spheres_.end(), std::back_inserter(shapes));
  for (auto shape : shapes)
  {
    glm::vec3 acc(0.f);
    glm::vec3 torque(0.f);

    for (auto f : g_forces_)
    {
      acc += f.second * shape->get_collidable().inv_mass;
    }
    for (auto f : g_torques_)
    {
      torque += f.second * shape->get_collidable().inv_mass;
    }

    // internal forces calculations
    glm::vec3 damping_force = -damping_ * shape->get_vel();
    acc += damping_force * shape->get_collidable().inv_mass;

    float angular_damping = 1.f / (1.f + damping_);

    // DUDU use semi-implicit euler
    shape->set_pos(shape->get_pos() + h * shape->get_vel());

    // linear momentum

    glm::vec3 P_dot(0.f);
    if (shape->get_collidable().inv_mass > .0001)
      P_dot = acc / shape->get_collidable().inv_mass;

    shape->get_collidable().P += h * P_dot;
    shape->set_vel(shape->get_collidable().P * shape->get_collidable().inv_mass);

    glm::mat3 R = glm::toMat3(shape->get_orientation());
    shape->get_collidable().IInv = R * shape->get_collidable().IBodyInv * glm::transpose(R);

    // angular_momentum
    glm::vec3 L_dot = torque;
    shape->get_collidable().L += L_dot * h * angular_damping;
    shape->set_angular_vel(shape->get_collidable().IInv * shape->get_collidable().L);

    glm::quat q = shape->get_orientation();
    shape->set_orientation(glm::normalize((q + 0.5f * glm::quat(0.f, shape->get_angular_vel()) * q * h)));

    shape->update_model_if_renderable(glm::vec3(shape->get_dims())); // DUDU identity orientation
  }

  // update reactphysics3d world
  for (reactphysics3d::CollisionBody *body : bodies_)
  {
    Shape *shape = reinterpret_cast<Shape *>(body->getUserData());
    reactphysics3d::Vector3 pos(shape->get_pos().x, shape->get_pos().y, shape->get_pos().z);
    reactphysics3d::Quaternion orientation(shape->get_orientation().w,
                                           reactphysics3d::Vector3(shape->get_orientation().x, shape->get_orientation().y, shape->get_orientation().z));
    reactphysics3d::Transform transform(pos, orientation);

    body->setTransform(transform);
  }
}

void Simulator::init()
{
  engine_.set_sphere_radius(sphere_rad_);
  engine_.set_world_dims(col_solver_->dims());
  engine_.init();

  add_global_force("gravity", GRAVITY);

  std::vector<size_t> elem_indices;
  const glm::vec3 dims = engine_.get_world_dims();
  float w = dims.x / 3.f;
  float h = dims.y / 3.f;
  float d = dims.z / 3.f;
  const bool small_start = false;

  // add spheres
  for (unsigned int i = 0; i < spheres_n_; ++i)
  {
    if (small_start)
      elem_indices.push_back(engine_.add_sphere(get_rand(), get_rand(), get_rand(), false, true));
    else
      elem_indices.push_back(engine_.add_sphere(get_rand(-w, w), get_rand(-h, h), get_rand(-d, d), false, true));
  }

  for (size_t ind : elem_indices)
  {
    Sphere *s = engine_.get_sphere(ind);
    s->set_initial_vel(glm::vec3(get_rand(-.2f, .2f), get_rand(-.2f, .2f), get_rand(-.2f, .2f)));
    spheres_.push_back(s);
  }

  // add boxes
  elem_indices.clear();
  for (unsigned int i = 0; i < boxes_n_; ++i)
  {
    elem_indices.push_back(engine_.add_box(glm::vec3(get_rand(-w, w), get_rand(-h, h), get_rand(-d, d)), glm::vec3(.4f, .4f, .4f), false, true));
  }

  for (size_t ind : elem_indices)
  {
    Box *b = engine_.get_box(ind);
    b->set_initial_vel(glm::vec3(get_rand(-.5f, .5f), get_rand(-.5f, .5f), get_rand(-.9f, .9f)));
    boxes_.push_back(b);
  }

  // add boundaries
  const glm::vec3 center = engine_.get_world_center();
  Box *floor = engine_.get_box(engine_.add_box(center + glm::vec3(0.f, -dims.y, 0.f), dims, true, false));
  Box *ceiling = engine_.get_box(engine_.add_box(center + glm::vec3(0.f, dims.y, 0.f), dims, true, false));
  Box *back = engine_.get_box(engine_.add_box(center + glm::vec3(0.f, 0.f, -dims.z), dims, true, false));
  Box *front = engine_.get_box(engine_.add_box(center + glm::vec3(0.f, 0.f, dims.z), dims, true, false));
  Box *right = engine_.get_box(engine_.add_box(center + glm::vec3(dims.x, 0.f, 0.f), dims, true, false));
  Box *left = engine_.get_box(engine_.add_box(center + glm::vec3(-dims.x, 0.f, 0.f), dims, true, false));

  boxes_.push_back(floor);
  boxes_.push_back(ceiling);
  boxes_.push_back(back);
  boxes_.push_back(front);
  boxes_.push_back(right);
  boxes_.push_back(left);

  for (size_t i = 0; i < boxes_.size(); ++i)
  {
    Box *box = boxes_[i];
    reactphysics3d::Vector3 pos(box->get_pos().x, box->get_pos().y, box->get_pos().z);
    reactphysics3d::Quaternion orientation(box->get_orientation().w,
                                           reactphysics3d::Vector3(box->get_orientation().x, box->get_orientation().y, box->get_orientation().z));
    reactphysics3d::Transform transform(pos, orientation);

    reactphysics3d::CollisionBody *body = world_->createCollisionBody(transform);
    body->setUserData(box);
    bodies_.push_back(body);
    const reactphysics3d::Vector3 halfExtents(box->get_dims().x * .5f, box->get_dims().y * .5f, box->get_dims().z * .5f);

    reactphysics3d::BoxShape *shape = physics_common_.createBoxShape(halfExtents);
    reactphysics3d::Collider *collider = body->addCollider(shape, reactphysics3d::Transform::identity());
  }

  for (size_t i = 0; i < spheres_.size(); ++i)
  {
    Sphere *sphere = spheres_[i];
    reactphysics3d::Vector3 pos(sphere->get_pos().x, sphere->get_pos().y, sphere->get_pos().z);
    reactphysics3d::Quaternion orientation(sphere->get_orientation().w,
                                           reactphysics3d::Vector3(sphere->get_orientation().x, sphere->get_orientation().y, sphere->get_orientation().z));
    reactphysics3d::Transform transform(pos, orientation);

    reactphysics3d::CollisionBody *body = world_->createCollisionBody(transform);
    body->setUserData(sphere);
    bodies_.push_back(body);
    reactphysics3d::SphereShape *shape = physics_common_.createSphereShape(sphere->rad);

    reactphysics3d::Collider *collider = body->addCollider(shape, reactphysics3d::Transform::identity());
  }

  debug_line_ = engine_.get_line(engine_.add_line(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
}

static void print_fps()
{
  namespace cr = std::chrono;
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
    handle_collisions();

    integrate();

    kinematics();

    engine_.draw();

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
  case GLFW_KEY_T:
  {
    if (action == GLFW_PRESS)
    {
      std::cout << "T press!!!\n";

      glm::vec3 torque(get_rand(-.1f, .1f), .5f, get_rand(-.1f, .1f));
      std::cout << "adding:(" << torque.x << "," << torque.y << "," << torque.z << "\n";
      add_global_torque("torque", torque);
    }
    else if (action == GLFW_RELEASE)
    {
      std::cout << "T release!!!\n";
      remove_global_torque("torque");
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
  case GLFW_KEY_R:
  {
    if (action == GLFW_PRESS)
    {
      std::cout << "R press!!!\n";
      boxes_[0]->get_collidable().P.z += .3f;
    }
  }
  break;
  case GLFW_KEY_E:
  {
    if (action == GLFW_PRESS)
    {
      std::cout << "R press!!!\n";
      boxes_[0]->get_collidable().P.z -= .3f;
    }
  }
  break;
  default:
  {
    // ignore
  }
  }
}
