#pragma once

#include "gl_incs.h"
#include "Collidable.h"


Collidable::Collidable(Type type,
                       float mass,
                       glm::mat3 IBody) : is_static_(mass <= 0.f),
                                          P(0.f),
                                          L(0.f)
{
  if (is_static_)
  {
    inv_mass = 0.f;
  }
  else
  {
    inv_mass = 1.f / mass;

    IBodyInv = glm::inverse(IBody);
  }
}
