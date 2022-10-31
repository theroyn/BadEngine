#pragma once

#include "gl_incs.h"
#include "BadEngine.h"
#include "Sphere.h"
#include "CollisionSolver.h"

#include <vector>
#include <map>
#include <reactphysics3d/reactphysics3d.h>


class Simulator
{
public:
  Simulator(unsigned int spheres_n);
  ~Simulator();

public:
  void add_global_force(const std::string &name, glm::vec3 f);
  void add_global_torque(const std::string &name, glm::vec3 f);
  void remove_global_force(const std::string &name);
  void remove_global_torque(const std::string &name);

public:
  void run();
  void init();

private:
  void integrate();
  void integrate_spheres(float h);
  void integrate_boxes(float h);
  void handle_collisions();
  void handle_sphere_collisions_naive_alg();

private:
  void key_callback(int key, int scancode, int action, int mods);

private:
  class CollisionOp : public reactphysics3d::CollisionCallback
  {
  public:
    CollisionOp(Simulator *parent) : parent_(parent) {}
    virtual void onContact(const CallbackData &callbackData) override;

  private:
    Simulator *parent_;
  };

private:
  float dampening_;
  const float base_h_;
  unsigned int spheres_n_;
  float sphere_rad_;
  double last_time_ = -1.;
  BadEngine engine_;
  std::map<std::string, glm::vec3> g_forces_;  // named forces
  std::map<std::string, glm::vec3> g_torques_; // named torques
  std::vector<Sphere *> spheres_;
  std::vector<Box *> boxes_;
  CollisionSolver *col_solver_;
  const sphere_coll_alg sphere_coll_alg_;
  reactphysics3d::PhysicsCommon physics_common_;
  reactphysics3d::PhysicsWorld *world_ = nullptr;
  CollisionOp collision_op_;
  std::unordered_map<size_t, reactphysics3d::CollisionBody *> bodies_;
};
