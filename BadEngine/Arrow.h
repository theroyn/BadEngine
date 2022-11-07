#pragma once

#include "gl_incs.h"
#include "utils.h"
#include "State.h"

class Arrow : public Shape
{
public:
  Arrow(Accessor<State> state_acc,
        const glm::vec3 &pos,
        const glm::vec3 &dims) : Shape(state_acc),
                                 pos_start(pos),
                                 dims(dims),
                                 vel_start(0.f)
  {
    set_vel(vel_start);
    set_pos(pos_start);
    static constexpr float ANGLE = 0.f;
    float half_angle = 0.5f * (utility::PI * 0.f);

    set_orientation(glm::normalize((glm::quat(cos(half_angle),
                                              sin(half_angle) * 1.f,
                                              sin(half_angle) * 0.f,
                                              sin(half_angle) * 0.f))));
  }

  void orient(const glm::vec3 &dir)
  {
    glm::vec3 v = glm::normalize(dir);
    glm::vec3 unit = glm::normalize(glm::vec3(0.f, 1.f, 0.f) - glm::vec3(0.f, 0.f, 0.f));
    float theta = acos(glm::dot(unit, v));
    glm::vec3 axis = glm::normalize(glm::cross(unit, v));

    if (abs(theta) > .0001f)
    {
      float half_angle = 0.5f * theta;

      set_orientation(glm::normalize((glm::quat(cos(half_angle),
                                                sin(half_angle) * axis.x,
                                                sin(half_angle) * axis.y,
                                                sin(half_angle) * axis.z))));
    }
  }

private:
  virtual Collidable create_collidable(float mass) const override
  {
    throw std::runtime_error("not implemented");
    glm::mat3 IBody = glm::identity<glm::mat3>(); // DUDU implement

    return Collidable(Collidable::Type::arrow, mass, IBody);
  }

public:
  glm::vec3 pos_start;
  glm::vec3 dims;
  glm::vec3 vel_start;
  float theta = 0.f;
};
