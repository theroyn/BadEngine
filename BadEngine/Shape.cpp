#pragma once

#include "Shape.h"

glm::vec3 Shape::get_pos() const
{
  return state_acc_.get().p;
}

void Shape::set_pos(const glm::vec3 &pos)
{
  state_acc_.get().p = pos;
}

glm::vec3 Shape::get_vel() const
{
  return state_acc_.get().v;
}

void Shape::set_vel(const glm::vec3 &v)
{
  state_acc_.get().v = v;
}

void Shape::add_renderable(Renderable r)
{
  r_ = r;
}

void Shape::update_model_if_renderable(const glm::quat &orientation, const glm::vec3 &dims)
{
  if (r_.has_value())
  {
    glm::mat4 model_trans(1.f);

    model_trans = glm::translate(model_trans,
                                 get_pos());
    model_trans *= glm::toMat4(orientation);
    model_trans = glm::scale(model_trans, dims);
    r_.value().update_model_transformation(model_trans);
  }
}
