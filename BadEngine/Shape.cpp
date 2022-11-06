#pragma once

#include "Shape.h"

glm::vec3 Shape::get_pos() const
{
  return pos_acc_.get();
}

void Shape::set_pos(const glm::vec3 &pos)
{
  pos_acc_.set(pos);
}

void Shape::add_renderable(Renderable r)
{
  r_ = r;
}

void Shape::render(const glm::quat &orientation, const glm::vec3 &dims)
{
  if (r_.has_value())
  {
    glm::mat4 model_trans(1.f);

    model_trans = glm::translate(model_trans,
                                 get_pos());
    model_trans *= glm::toMat4(orientation);
    model_trans = glm::scale(model_trans, dims);
    r_.value().Render(model_trans);
  }
}
