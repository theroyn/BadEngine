#include <iostream>
//#include <BadEngine.h>
#include "Simulator.h"

int main(int argc, char *argv[])
{
  unsigned int spheres_n = 3000;

  if (argc == 2)
  {
    int n = atoi(argv[1]);
    if (n > 0)
      spheres_n = n;
  }

  Simulator sim(spheres_n);

  sim.init();

  sim.run();
}
