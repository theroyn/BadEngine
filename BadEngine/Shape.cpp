#pragma once

#include "Shape.h"

#include <stdexcept>

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

glm::vec3 Shape::get_angular_vel() const
{
  return state_acc_.get().angular_vel;
}

void Shape::set_angular_vel(const glm::vec3 &w)
{
  state_acc_.get().angular_vel = w;
}

glm::quat Shape::get_orientation() const
{
  return state_acc_.get().orientation;
}

void Shape::set_orientation(const glm::quat &q)
{
  state_acc_.get().orientation = q;
}

glm::vec3 Shape::get_dims() const
{
  return dims_;
}

void Shape::add_renderable(Renderable r)
{
  r_ = r;
}

void Shape::add_collidable(float mass)
{
  c_ = create_collidable(mass);
}

bool Shape::has_collidable() const
{
  return c_.has_value();
}

Collidable &Shape::get_collidable()
{
  if (!has_collidable())
  {
    throw std::runtime_error("no collidable");
  }

  return c_.value();
}

Collidable Shape::get_collidable() const
{
  if (!has_collidable())
  {
    throw std::runtime_error("no collidable");
  }

  return c_.value();
}

void Shape::update_model_if_renderable(const glm::vec3 &dims)
{
  if (r_.has_value())
  {
    glm::mat4 model_trans(1.f);

    model_trans = glm::translate(model_trans,
                                 get_pos());
    model_trans *= glm::toMat4(get_orientation());
    model_trans = glm::scale(model_trans, dims);
    r_.value().update_model_transformation(model_trans);
  }
}
