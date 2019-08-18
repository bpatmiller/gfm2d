#include "simulation.hpp"
#include "export_data.hpp"
#include "levelset_methods.hpp"
#include "particle_levelset_method.hpp"

/** TODO Returns a timestep that ensures the simulation is stable */
float Simulation::cfl() {
  float reciprocal = (u.infnorm() + v.infnorm()) / h;
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
  // delete old datafiles, fix after initializing
  clear_exported_data();
  project_phi(fluids);
  while (time_elapsed < max_t) {
    export_simulation_data(fluids, time_elapsed, frame_number);
    frame_number += 1;
    if (time_elapsed + timestep > max_t)
      timestep = max_t - time_elapsed;
    // break the timestep up
    float t = 0;
    while (t < timestep) {
      float substep = std::min(cfl(), timestep / 10.0f);
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
  assert(dt > 0);
  for (auto &f : fluids) {
    advect_phi(u, v, f.phi, dt);
    advect_particles(f, vel, solid_phi, dt);
    // correct
    reinitialize_phi(fluids);
    // correct
    // adjust radii
    // reseed
  }
  project_phi(fluids);

  // advect_velocity(dt);
  add_gravity(dt);

  // enforce boudnaries
  // project pressure
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

void Simulation::add_gravity(float dt) {
  for (auto &face : v.data) {
    face -= 9.8 * dt;
  }
}

void Simulation::advect_velocity(float dt) {
  Array2f new_u(u);
  Array2f new_v(v);

  for (auto it = u.begin(); it != u.end(); it++) {
    vec2 new_position = rk4(it.wp(), vel, -dt);
    new_u(it.ij()) = u.value_at(new_position);
  }

  for (auto it = v.begin(); it != v.end(); it++) {
    vec2 new_position = rk4(it.wp(), vel, -dt);
    new_v(it.ij()) = v.value_at(new_position);
  }

  u = new_u;
  v = new_v;
}