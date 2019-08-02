#include "SphereGridMap.h"


void SphereGridMap::update_map(const std::vector<Sphere *> &spheres)
{
  map_.clear();

  for (Sphere *s : spheres)
  {
    insert(s);
  }
}

void SphereGridMap::get_by_coords(std::list<Sphere *> &vec, const glm::uvec3 &coords)
{
  auto r = map_.equal_range(get_flat_idx(coords));

  for (auto it = r.first; it != r.second; ++it)
  {
    vec.push_back((*it).second);
  }
}

void SphereGridMap::get_neighbours(const Sphere *s, std::list<Sphere *> &res)
{
  auto coords = get_3d_idx(s->pos);

  size_t x_inf = (coords.x > 0) ? coords.x - 1 : coords.x;
  size_t x_sup = (coords.x + 1 <= world_cells_n_.x) ? coords.x + 1 : coords.x;

  // Iterate [coords-1, coords+1].
  for (size_t x = x_inf;
       x <= x_sup;
       ++x)
  {
    size_t y_inf = (coords.y > 0) ? coords.y - 1 : coords.y;
    size_t y_sup = (coords.y + 1 <= world_cells_n_.y) ? coords.y + 1 : coords.y;

    for (size_t y = y_inf;
         y <= y_sup;
         ++y)
    {
      size_t z_inf = (coords.z > 0) ? coords.z - 1 : coords.z;
      size_t z_sup = (coords.z + 1 <= world_cells_n_.z) ? coords.z + 1 : coords.z;

      for (size_t z = z_inf;
           z <= z_sup;
           ++z)
      {
        get_by_coords(res, glm::uvec3(x, y, z));
      }
    }
  }
}

size_t SphereGridMap::insert(Sphere *s)
{
  size_t idx = get_flat_idx(s->pos);

  map_.emplace(idx, s);

  return idx;
}

/**
 * (0, 0, 0)=>0, (world_w_n, 0, 0)=>world_w_n.
 *
 * (0, 1, 0)=>world_w_n+1, (world_w_n, 1, 0)=>2*(world_w_n + 1) - 1.
 *
 * (world_w_n, world_h_n, 0)=>(world_h_n+1)*(world_w_n + 1) - 1, (0, 0, 1)=>(world_h_n+1)*(world_w_n + 1).
 * (h+1)(w+1) - 1 = (h+1)w + h + 1
 * (world_w_n, world_h_n, 1)=>2 * ((world_h_n+1)*(world_w_n + 1)) - 1, (world_w_n, world_h_n, world_d_n)=>(h+1)*(w+1)*(d+1) - 1.
 * 
 * (x, y, 0)=> x + y*(world_w_n+1)
 * 
 * (x, y, z)=> x + y*(world_w_n+1) + z*(world_h_n+1)*(world_w_n + 1)
 * d(h+1)(w+1) + h(w+1)+w+1 -1 = d(h+1)(w+1)+(w+1)(h+1) - 1 = (h+1)*(w+1)*(d+1) - 1.
 */
size_t SphereGridMap::get_flat_idx(size_t x, size_t y, size_t z)
{
  size_t res = x +
               y * (world_cells_n_.x + 1) +
               z * (world_cells_n_.y + 1)*(world_cells_n_.x + 1);

  return res;
}

size_t SphereGridMap::get_flat_idx(const glm::uvec3 &coords)
{

  return get_flat_idx(coords.x, coords.y, coords.z);
}

size_t SphereGridMap::get_flat_idx(const glm::vec3 &pos)
{
  size_t res = get_flat_idx(get_3d_idx(pos));

  return res;
}

glm::uvec3 SphereGridMap::get_3d_idx(const glm::vec3 &pos)
{
  //const glm::uvec3 coords = pos / cell_dims_;
  const glm::uvec3 coords = (pos + (world_dims_ / 2.f)) / cell_dims_;

  return coords;
}