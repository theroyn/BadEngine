#include <iostream>

#include "Simulator.h"

int main(int argc, char *argv[])
{
  try
  {
    unsigned int spheres_n = 9;
    unsigned int boxes_n = 9;

    if (argc == 2)
    {
      int n = atoi(argv[1]);
      if (n > 0)
        spheres_n = n;
    }

    Simulator sim(spheres_n, boxes_n);

    sim.init();

    sim.run();
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what();
  }
}
