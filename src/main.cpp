#include "settings.hpp"

int main() {
  Simulation sim;
  initialize_simulation(sim);
  sim.print_information();

  sim.run();
  return 0;
}