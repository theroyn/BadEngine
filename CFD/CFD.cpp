#include <iostream>
//#include <BadEngine.h>
#include "Simulator.h"


int main()
{
  Simulator sim;

  sim.init();

  sim.run();
   /* std::vector<int> spheres_indices;
    std::vector<Sphere *> spheres;
    spheres_indices.push_back(engine.add_sphere(2.f, 1.f, -1.5f));
    spheres_indices.push_back(engine.add_sphere(0.f, 1.f, -1.5f));
    spheres_indices.push_back(engine.add_sphere(-2.f, 1.f, -1.5f));
    for (int ind : spheres_indices)
    {
      engine.set_sphere_velocity(ind, .005f, .005f, -.001f);
      engine.set_sphere_acc(ind, .001f, .001f, -.001f);
      spheres.push_back(engine.get_sphere(ind));
    }

    float h = .01f;*/
    
    /*while (!engine.loop_done())
    {
      engine.draw();
      static bool move_s = true;
      if (move_s)
      {
        for (int ind : spheres_indices)
        {
          Sphere *s = engine.get_sphere(ind);

          glm::vec3 vel = s->vel + (h * s->acc);
          glm::vec3 pos = s->pos + (h * s->vel);
          engine.set_sphere_velocity(ind, vel.x, vel.y, vel.z);
          engine.set_sphere_pos(ind, pos.x, pos.y, pos.z);
        }
      }
    }*/
}
