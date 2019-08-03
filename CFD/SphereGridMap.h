#pragma once


#include <unordered_map>
#include <gl_incs.h>
#include <vector>
#include <list>
#include "Sphere.h"

class SphereGridMap
{
public:
  SphereGridMap(float  rad, glm::vec3 world_dims) : rad_(rad),
    world_dims_(world_dims),
    cell_dims_(2 * rad),
    world_cells_n_(world_dims_ / cell_dims_)
  { }

public:
  void update_map(const std::vector<Sphere *> &spheres);
  void get_neighbours(const Sphere *s, std::list<Sphere *> &res) const;

private:
  size_t insert(Sphere *s);
  size_t get_flat_idx(const glm::vec3 &pos) const;

private:
  glm::uvec3 get_3d_idx(const glm::vec3 &pos) const;
  size_t get_flat_idx(size_t a, size_t b, size_t c) const;
  size_t get_flat_idx(const glm::uvec3 &coords) const;
  void get_by_coords(std::list<Sphere *> &vec, const glm::uvec3 &coords) const;

private:
  std::unordered_multimap<size_t, Sphere *> map_;
  const float rad_;
  glm::vec3 world_dims_; // x=w, y=h, z=d
  const glm::vec3 cell_dims_; // x=w, y=h, z=d
  // x=w, y=h, z=d
  //size_t world_w_n_, world_h_n_, world_d_n_;
  glm::uvec3 world_cells_n_;
};
