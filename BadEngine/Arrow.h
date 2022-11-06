#pragma once

#include "gl_incs.h"

#include "utils.h"

class Arrow : public Shape
{
public:
  Arrow(Accessor<glm::vec3> pos_acc,
        Accessor<glm::vec3> vel_acc,
        const glm::vec3 &pos,
        const glm::vec3 &dims) : Shape(pos_acc),
                                 pos_start(pos),
                                 dims(dims),
                                 vel_start(0.f),
                                 vel_acc_(vel_acc)
  {
    set_vel(vel_start);
    set_pos(pos_start);
    static constexpr float ANGLE = 0.f;
    float half_angle = 0.5f * (utility::PI * 0.f);
    orientation.w = cos(half_angle);
    orientation.x = sin(half_angle) * 1.f;
    orientation.y = sin(half_angle) * 0.f;
    orientation.z = sin(half_angle) * 0.f;
    orientation = glm::normalize(orientation);
  }

  glm::vec3 get_vel() const { return vel_acc_.get(); }

  void set_vel(const glm::vec3 &v)
  {
    vel_acc_.set(v);
  }

  void orient(const glm::vec3 &dir)
  {
    glm::vec3 v = glm::normalize(dir);
    glm::vec3 unit = glm::normalize(glm::vec3(0.f, 1.f, 0.f) - glm::vec3(0.f, 0.f, 0.f));
    float theta = acos(glm::dot(unit, v));
    glm::vec3 axis = glm::normalize(glm::cross(unit, v));

    if (abs(theta) < .0001f)
    {
      orientation = glm::quat(1.f, glm::vec3(0.f, 0.f, 0.f));
    }
    else
    {
      float half_angle = 0.5f * theta;
      orientation.w = cos(half_angle);
      orientation.x = sin(half_angle) * axis.x;
      orientation.y = sin(half_angle) * axis.y;
      orientation.z = sin(half_angle) * axis.z;
      orientation = glm::normalize(orientation);
    }
  }

  glm::vec3 pos_start;
  glm::vec3 dims;
  glm::vec3 vel_start;
  float theta = 0.f;
  glm::quat orientation;
  Accessor<glm::vec3> vel_acc_;
};
