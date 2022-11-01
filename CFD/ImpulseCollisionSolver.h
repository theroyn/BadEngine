#pragma once

#include "gl_incs.h"

#include <vector>
#include <reactphysics3d/reactphysics3d.h>

class Simulator;
struct Box;

class ImpulseCollisionSolver : public reactphysics3d::CollisionCallback
{
public:
  ImpulseCollisionSolver(Simulator *parent) : parent_(parent) {}
  virtual void onContact(const CallbackData &callbackData) override;
  void solve();
  bool had_collisions() const { return had_collisions_; }
  bool has_contacts() const { return contact_pairs_.size() > 0; }
  void clear();

private:
  struct ContactPointData
  {
    Box *box1;
    Box *box2;
    float penetration_depth;
    glm::vec3 n;
    glm::vec3 p;
    glm::vec3 pt;
  };

private:
  bool colliding(const ContactPointData &contact_point);
  void solve_single_collision(const ContactPointData &contact_point);

private:
  Simulator *parent_;
  bool had_collisions_ = false;
  std::vector<ContactPointData> contact_pairs_;
};
