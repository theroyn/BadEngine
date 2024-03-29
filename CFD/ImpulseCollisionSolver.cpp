
#include "ImpulseCollisionSolver.h"
#include "Shape.h"

static inline constexpr float THRESHOLD = .01f;
static inline constexpr float EPSILON = .5f;

static reactphysics3d::Vector3 GlmToRp3d(const glm::vec3 &v)
{
  return reactphysics3d::Vector3(v.x, v.y, v.z);
}

static glm::vec3 Rp3dToGlm(const reactphysics3d::Vector3 &v)
{
  return glm::vec3(v.x, v.y, v.z);
}

void ImpulseCollisionSolver::onContact(const CallbackData &callbackData)
{
  uint32_t contact_pairs_num = callbackData.getNbContactPairs();
  for (size_t i = 0; i < contact_pairs_num; ++i)
  {
    auto contact_pair = callbackData.getContactPair(i);
    Shape *shape1 = reinterpret_cast<Shape *>(contact_pair.getBody1()->getUserData());
    Shape *shape2 = reinterpret_cast<Shape *>(contact_pair.getBody2()->getUserData());

    for (uint32_t i = 0; i < contact_pair.getNbContactPoints(); ++i)
    {
      ContactPoint contact_point = contact_pair.getContactPoint(i);
      const glm::vec3 p = Rp3dToGlm(contact_pair.getBody1()->getWorldPoint(contact_point.getLocalPointOnCollider1()));
      const glm::vec3 pt = Rp3dToGlm(contact_pair.getBody2()->getWorldPoint(contact_point.getLocalPointOnCollider2()));
      contact_pairs_.emplace_back(ContactPointData{
          shape1,
          shape2,
          contact_point.getPenetrationDepth(),
          // normal dir switched to b2-->b1
          -Rp3dToGlm(contact_point.getWorldNormal()),
          p,
          pt,
      });
    }
  }
}

glm::vec3 get_local_p_vel(Shape *s, const glm::vec3 &loc_p)
{
  return s->get_vel() + glm::cross(s->get_angular_vel(), loc_p - s->get_pos());
}

bool ImpulseCollisionSolver::colliding(const ContactPointData &contact_point)
{
  // normal dir switched to b2-->b1
  const glm::vec3 &n = contact_point.n;
  glm::vec3 v1 = get_local_p_vel(contact_point.shape1, contact_point.p);
  glm::vec3 v2 = get_local_p_vel(contact_point.shape2, contact_point.p);
  float vrel = glm::dot(n, v1 - v2);
  if (vrel > THRESHOLD) // separating
  {
    return false;
  }
  // DUDU handling for resting contact
  //else if (vrel > -THRESHOLD) // resting
  //{
  //  return false;
  //}
  else // colliding
  {
    return true;
  }
}

void ImpulseCollisionSolver::solve_single_collision(const ContactPointData &contact_point)
{
  // normal dir switched to b2-->b1
  const glm::vec3 &n = contact_point.n;
  glm::vec3 p1dot = get_local_p_vel(contact_point.shape1, contact_point.p);
  glm::vec3 p2dot = get_local_p_vel(contact_point.shape2, contact_point.p);
  glm::vec3 r1 = contact_point.p - contact_point.shape1->get_pos();
  glm::vec3 r2 = contact_point.p - contact_point.shape2->get_pos();
  float vrel = glm::dot(n, p1dot - p2dot);
  float numerator = -(1.f + EPSILON) * vrel;

  Collidable &c1 = contact_point.shape1->get_collidable();
  Collidable &c2 = contact_point.shape2->get_collidable();
  float t1 = c1.inv_mass;
  float t2 = c2.inv_mass;
  float t3 = glm::dot(n, (glm::cross(c1.IInv * (glm::cross(r1, n)), r1)));
  float t4 = glm::dot(n, (glm::cross(c2.IInv * (glm::cross(r2, n)), r2)));

  float j = numerator / (t1 + t2 + t3 + t4);
  glm::vec3 j_force = j * n;

  c1.P += j_force;
  c2.P -= j_force;
  c1.L += glm::cross(r1, j_force);
  c2.L -= glm::cross(r2, j_force);

  contact_point.shape1->set_vel(c1.P * c1.inv_mass);
  contact_point.shape2->set_vel(c2.P * c2.inv_mass);
  contact_point.shape1->set_angular_vel(c1.IInv * c1.L);
  contact_point.shape2->set_angular_vel(c2.IInv * c2.L);
}

void ImpulseCollisionSolver::solve()
{
  for (ContactPointData &contact_point : contact_pairs_)
  {
    if (colliding(contact_point))
    {
      solve_single_collision(contact_point);
      had_collisions_ = true;
    }
  }
}

void ImpulseCollisionSolver::clear()
{
  had_collisions_ = false;
  contact_pairs_.clear();
}
