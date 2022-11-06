#pragma once

#include "gl_incs.h"

#include <functional>
#include "Accessor.h"

class Renderable
{
  Accessor<glm::mat4> updater_;

public:
  Renderable(Accessor<glm::mat4> updater) : updater_(updater)
  {
  }
  void update_model_transformation(const glm::mat4 &m) { updater_.set(m); }
};