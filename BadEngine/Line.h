#pragma once

#include "gl_incs.h"

struct Line
{
  Line(const glm::vec3 &start, const glm::vec3 &end) : start(start), end(end), color(0.f, 1.f, 0.f) {}

  glm::vec3 start;
  glm::vec3 end;
  glm::vec3 color;
};
