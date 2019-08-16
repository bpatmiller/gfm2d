#include "simulation.hpp"
#include "levelset_methods.hpp"
#include "particle_levelset_method.hpp"

/** TODO Returns a timestep that ensures the simulation is stable */
float Simulation::cfl() {
  float reciprocal = (u.max() + v.max()) / h;
  return 1.0 / reciprocal;
}

/** Runs the main simulation loop. Exports simulation data at each timestep,
 * and breaks the timestep into substeps based on the CFL condition
 * time_elapsed - current runtime of the simulation
 * max_t        - total amount of time the simulation will run
 * timestep     - amount of time between "frames"
 * t            - tracks the amount of time traversed in a given frame
 * substep      - a length of time given by cfl() */
void Simulation::run() {
  while (time_elapsed < max_t) {
    if (time_elapsed + timestep > max_t)
      timestep = max_t - time_elapsed;
    // break the timestep up
    float t = 0;
    while (t < timestep) {
      float substep = cfl();
      if (t + substep > timestep)
        substep = timestep - t;
      advance(substep);
      t += substep;
    }
    time_elapsed += timestep;
  }
}

/* The central method in the Simulation class. This performs all of our
 * computations for a given timestep that it assumed to be safe. */
void Simulation::advance(float dt) {
  for (auto &f : fluids) {
    advect_phi(u, v, f.phi, dt);
    advect_particles(f, vel, solid_phi, dt);
  }
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