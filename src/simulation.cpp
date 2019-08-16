#include "simulation.hpp"

void Simulation::run() {
    while (t < max_t) {
        if (t + timestep > max_t) timestep = max_t - t;
        advance(timestep);
        t+=timestep;
    }
}

void Simulation::advance(float dt) {
}

void Simulation::get_fluid_ids() {
  Array2f min_phi(sx, sy, -0.5, -0.5, h);
  min_phi.set(99999.9);

  for (uint n = 0; n < fluids.size(); n++) {
    auto &f = fluids[n];
    for (auto it = fluid_id.begin(); it != fluid_id.end(); it++) {
      vec2 ij = it.ij();
      if (f.phi(ij) < min_phi(ij)) {
        min_phi(ij) = f.phi(ij);
        fluid_id(ij) = n;
      }
    }
  }
}