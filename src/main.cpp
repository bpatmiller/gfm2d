#include "settings.hpp"

int main() {
  Simulation sim;
  initialize_simulation(sim);

  sim.run();
  return 0;
}